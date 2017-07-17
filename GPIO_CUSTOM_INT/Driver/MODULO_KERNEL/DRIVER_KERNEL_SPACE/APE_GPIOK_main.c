/**
  ******************************************************************************
  * @file    APE_GPIOK_main.c
  * @author  Alfonso,Pierluigi,Erasmo (APE)
  * @version V1.0
  * @date    13-Luglio-2017
  * @brief	File che implementa il modulo KERNEL.
  *	@addtogroup DRIVER
  * @{
  * @addtogroup KERNEL
  * @{
  * @brief Modulo kernel per la periferica APE_GPIOK.
  * @details Il modulo esporta le principali funzioni che devono essere fornite
  *			di norma da un modulo kernel.
  *			Il driver istanzia tanti device file sotto /dev quante sono le periferiche
  *			collegate e compatibili al modulo (come dichiarato nel device tree).
  *			Un primo limite del modulo sta nel numero di periferiche gestibili,
  *			limitato dalla macro MAX_NUM_DEVICES.
  *			I device sono gestiti come device a carattere e sono mantenuti in
  *			memoria mediante un array di puntatori a struttura APE_GPIOK_dev_t.
  *			L'array viene inizializzato alla prima chiamata della funzione probe,
  *			che avviene ogni volta che un device viene caricato. Per le successive chiamata,
  *			la funzione probe di occupa solo di creare una nuova struttura e associarla
  *			all'array.
  ******************************************************************************
  */

/* Includes -------------------------------------------------------------------*/
#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/proc_fs.h>
#include <linux/kernel.h> /* necessario per printk() */
#include <linux/slab.h> /* necessario per kmalloc() */
#include <linux/errno.h> /* necessario per i codici di errore */
#include <linux/types.h>
#include <linux/fcntl.h>
#include <linux/poll.h> /* necessario per utilizzare poll_table*/
#include <asm/uaccess.h> /* necessario per copy_from/to_user */
#include <linux/platform_device.h> /*necessario per platform_device*/
#include <asm/io.h>
#include <linux/unistd.h>
#include <linux/of_device.h>
#include <linux/of_platform.h>
#include <linux/of_address.h>
#include <linux/sched.h>
#include <linux/spinlock.h>
#include <linux/interrupt.h>
#include <linux/device.h>

#include "APE_GPIOK_includes.h"

/* Macro ----------------------------------------------------------------------*/
#define DRIVER_NAME     "APE_GPIOK"	/*!< Nome del driver*/
#define MAX_NUM_DEVICES 10	/*!< Numero massimo device numbers contigui da assegnare*/

/* Variabili Globali ----------------------------------------------------------*/
struct class *APE_GPIOK_class;			/*!< Classe del device*/
APE_GPIOK_dev_t  **device_array; 		/*!< Array di device da caricare*/
static int num_of_devices = 0;			/*!< Numero di device attualmente gestiti dal modulo*/

/* Prototipi delle funzioni----------------------------------------------------*/
static int APE_GPIOK_open(struct inode *, struct file *);
static int APE_GPIOK_release(struct inode *, struct file *);
static ssize_t APE_GPIOK_read(struct file *, char *, size_t , loff_t *);
static ssize_t APE_GPIOK_write(struct file *, const char *, size_t , loff_t *);
static unsigned int APE_GPIOK_poll(struct file *filp, poll_table *wait);
static irqreturn_t APE_GPIOK_handler(int irq, struct pt_regs * regs);

/**
  * @brief	Stuttura delle operazioni esportate dal modulo.
  */
struct file_operations APE_GPIOK_fops = {
	.owner = THIS_MODULE,			/*!< Modulo proprietario della struttura*/
	.open = APE_GPIOK_open,			/*!< Metodo open del modulo*/
	.release = APE_GPIOK_release,	/*!< Metodo release del modulo*/
	.read = APE_GPIOK_read,			/*!< Metodo read del modulo*/
	.write = APE_GPIOK_write,		/*!< Metodo write del modulo*/
	.poll = APE_GPIOK_poll			/*!< Metodo poll del modulo*/
};

/**
  * @brief	Stuttura che identifica tutti i device del DTB che matchano
  *			la corrispondente stringa "compatible".
  */
struct of_device_id APE_GPIOK_match[] = {
	{.compatible = "APE_GPIOK"},	/*!< La medesima stringa va copiata nel DTB*/
	{},
};

/**
  * @brief	Inserisce la entry del device nella tabella dei device gestita dal kernel.
  */
MODULE_DEVICE_TABLE(of, APE_GPIOK_match);

/**
  * @brief	Apre il file specifico del device e collega la struttura dati del device
  *			al campo private date del descrittore del file.
  *	@param	inode: puntatore struttura inode che contiene il campo i_cdev
  *	@param	file: puntatore struttura file per accedere ai private_date
  *	@retval	0 sempre
  */
int APE_GPIOK_open(struct inode *inode, struct file *file){

	APE_GPIOK_dev_t *APE_GPIOK_devp;

	printk(KERN_INFO "APE_GPIOK_open\n");

	/* Tramite l'inode ricaviamo la struttura APE_GPIOK_dev che contiene
	 * a sua volta la struttura cdev.
	 */
	APE_GPIOK_devp = container_of(inode->i_cdev, APE_GPIOK_dev_t, cdev);

	/* Ottenuto il puntatore alla struttura device, lo si salva nel campo
	 * private_date della file structure per un piu' facile accesso.
	 */
	file->private_data = APE_GPIOK_devp;

	return 0;
}

/**
  * @brief	Dealloca tutte le strutture inizializzate dalla open. In questo caso non fa niente.
  *	@param	inode: puntatore struttura inode che contiene il campo i_cdev
  *	@param	file: puntatore struttura file per accedere ai private_date
  *	@retval	0 sempre
  */
int APE_GPIOK_release(struct inode *inode, struct file *file){

	printk(KERN_INFO "APE_GPIOK_release\n");

	return 0;
  }

/**
  *	@brief	Trasferisce dati dal device verso un buffer user-space.
  *	@details Permette di realizzare sia una lettura bloccante, che prevede la sospensione del processo,
  *			che non bloccante. Cio' e' gestito con l'apposito flag read_flag della struttura dati del device.
  *			La funzione read, una volta effettuata la lettura, deve sempre incrementare
  *			il valore ppos, che specifica la posizione all'interno del file da cui partirà
  *			una nuova operazione. In questo caso ppos e' utilizzato come offset da sommare
  *			all'indirizzo base del device e viene incrementato alla chiusura.
  *			Qualora si volesse invocare la pread, il kernel ignorerebbe qualsiasi cambiamento
  *			fatto da questa funzione su campo ppos.
  * @param	file: puntatore alla struttura file.
  *	@param	buf: puntatore al buffer user-space in cui trasferire i dati letti.
  *	@param	count: lunghezza del trasferimento richiesto.
  * @param	ppos: puntatore alla posizione corrente nel file. Utilizzato come
  *			offset da sommare all'indirizzo presso cui si vuole leggere.
  *	@retval	1 in assenza di errori.
  */
ssize_t APE_GPIOK_read(struct file *file, char *buf, size_t count, loff_t *ppos){

	//loff_t offset;
	APE_GPIOK_dev_t *devp;
	unsigned int value = 0; /* Valore letto dalla periferica*/
	unsigned long *read_addr;

	printk(KERN_INFO "APE_GPIOK_read\n");

	devp = file->private_data;
	read_addr = (devp->base_addr) + *ppos;

	/* Effettua la lettura in base alla modalita' definita in f_flags*/
	if((file->f_flags & O_NONBLOCK) == 0){
		/* Modalita' di lettura BLOCCANTE*/
		printk(KERN_INFO "Lettura bloccante\n");

		/* Effettua la wait sulla variabile condition*/
		printk(KERN_INFO "Il processo %i (%s) viene bloccato in lettura\n",current->pid, current->comm);
		wait_event_interruptible(devp->read_queue, (devp->read_flag & EN_BLOC_READ)== 1);
		printk(KERN_INFO "Il processo %i (%s) viene risvegliato in lettura\n",current->pid, current->comm);

		/* Acquisisci il lock*/
		spin_lock(&devp->read_flag_sl);

		/* Resetta il flag di abilitazione alla lettura*/
		devp->read_flag &= ~(EN_BLOC_READ);

		/* Rilascia il lock*/
		spin_unlock(&devp->read_flag_sl);

	} else {
		/* Modalita' di lettura NON BLOCCANTE*/
		printk(KERN_INFO "Lettura non bloccante\n");

		/* Acquisisce il lock*/
		spin_lock(&devp->read_flag_sl);

		/* Resetta il flag di abilitazione alla lettura*/
		devp->read_flag &= ~(EN_NONBLOC_READ);

		/* Rilascia il lock*/
		spin_unlock(&devp->read_flag_sl);
	}

	/* Completa la lettura del dato*/
	value = ioread32(read_addr);
	printk(KERN_INFO "Valore Letto: %u\n",value);

	/* Incrementa la posizione*/
	*ppos = *ppos + 1;

	/* Trasferisce i dati allo spazio utente*/
	if(copy_to_user(buf, &value, 1)){
		return -EFAULT;
	}

	return 1;
}

/**
  *	@brief	Trasferisce dati buffer user-space al device.
  * @details Come per la funzione di read, anche la write incremente il valore di ppos,
  *			che viene ignorato dal kernel qualora si usi la pwrite al livello utente.
  * @param	file: puntatore alla struttura file.
  *	@param	buf: puntatore al buffer user-space da cui prendere i dati.
  *	@param	count: lunghezza del trasferimento richiesto.
  * @param	ppos: ppos: puntatore alla posizione corrente nel file. Utilizzato come
  *			offset da sommare all'indirizzo presso cui si vuole scrivere.
  *	@retval	1 in assenza di errori.
  */
ssize_t APE_GPIOK_write(struct file *file, const char *buf, size_t count, loff_t *ppos){

	APE_GPIOK_dev_t *devp;
	unsigned char value;
	unsigned long *write_addr;

    printk(KERN_INFO "APE_GPIOK_write\n");

    devp = file->private_data;
	write_addr = (devp->base_addr) + *ppos;

    /* Trasferisce i dati allo spazio kernel*/
    if(copy_from_user(&value, buf, count)){
		return -EFAULT;
	}

	/* Completa la scrittura del dato*/
	iowrite32(value, write_addr);
	printk(KERN_INFO "Valore Scritto: %u\n",value);

    /* Incrementa la posizione*/
	*ppos = *ppos + 1;

    return count;
}

/**
  *	@brief	Verifica se un'operazione di read o write su uno o piu' file
  *			descriptor sara' bloccante. Questa funzione deve:
  *			- Chiamare poll_wait su una o piu' code;
  *			- Restituire una maschera che indichi le operazioni che
  *			possono essere eseguite immediatamente senza essere bloccate.
  *	@param	file: puntatore alla struttura file
  *	@param	wait: puntatore poll_table, caricata assieme ad ogni coda che possa
  			risvegliare il processo
  *	@retval	mask: maschera delle operazioni eseguibili immediatamente
  */
unsigned int APE_GPIOK_poll(struct file *file, poll_table *wait){

	APE_GPIOK_dev_t *devp;
	unsigned int mask;

	printk(KERN_INFO "APE_GPIOK_poll\n");

	devp = file->private_data;
	mask = 0;

	/* La poll_wait aggiunge una nuova coda di attesa alla poll_table*/
	printk(KERN_INFO "Il processo %i (%s) viene bloccato\n",current->pid, current->comm);
	poll_wait(file, &devp->poll_queue,  wait);
	printk(KERN_INFO "Il processo %i (%s) viene risvegliato\n",current->pid, current->comm);

	/* Acquisisce il lock*/
	spin_lock(&devp->read_flag_sl);

	/* Verifica se il dato e' pronto e setta la maschera*/
	if(devp->read_flag & EN_BLOC_READ){
		/* Readable device*/
		mask = POLLIN | POLLRDNORM;
	}

	/* Rilascia il lock*/
	spin_unlock(&devp->read_flag_sl);

	return mask;
}

/**
  *	@brief	ISR della periferica, le interrupt sono disabilitate durante la sua
  *			esecuzione (fast interrupt handler).
  * @details Si fa notare che la periferica e' progettata in modo tale che, se la direzione
  *			 e' impostata in scrittura sul pad, non vengono mai sollevate eccezioni.
  *			Dunque e' possibile evitare di restituire IRQ_NONE secondo la convenzione.
  *	@param	irq: interrupt number
  *	@param	regs: contiene uno snapshot del contesto del processore prima della ISR
  *	@retval	Valore che indica se c'e' stata effettivamente una interrupt da servire,
  			in caso affermativo deve valere IRQ_HANDLED altrimenti IRQ_NONE
  */
static irqreturn_t APE_GPIOK_handler(int irq, struct pt_regs * regs){

	unsigned long flags;
	int i;
	IER_status_t int_status;
	APE_GPIOK_dev_t *devp;

	/* identifica il device mediante la linea irq*/
	for(i = 0; i < num_of_devices; i++){
		if(device_array[i]->irq_number == irq){
			devp = device_array[i];
			break;
		}
	}

	/* Disabilita le interruzioni*/
	APE_GPIOK_saveInt(devp,&int_status);
	APE_GPIOK_writeIER(devp,0x0);
	printk(KERN_INFO "ISR in esecuzione\n");

	/* Accesso al flag abilitazione alla lettura ------------------------------*/

	/* Acquisisce il lock e disabilita le interrupt, lo stato e' salvato in flags*/
	spin_lock_irqsave(&devp->read_flag_sl, flags);

	/* Ripristina flag di abilitazione alla lettura (sia sincrona che asincrona)*/
	devp->read_flag = EN_BLOC_READ | EN_NONBLOC_READ;

	/* Rilascia il lock, riabilita le interrupt e rispristina lo stato*/
	spin_unlock_irqrestore(&devp->read_flag_sl, flags);

	/* Accesso al contatore delle interrupt totali ----------------------------*/

	/* Acquisisce il lock e disabilita le interrupt, lo stato e' salvato in flags*/
	spin_lock_irqsave(&devp->num_interrupts_sl, flags);

	/* Incrementa il contatore delle interruzioni avvenute*/
	devp->num_interrupts = devp->num_interrupts+1;

	/* Rilascia il lock, riabilita le interrupt e rispristina lo stato*/
	spin_unlock_irqrestore(&devp->num_interrupts_sl, flags);

	/* Risveglio processi -----------------------------------------------------*/
	printk(KERN_DEBUG "Processo %i (%s) risveglia i lettori...\n",current->pid, current->comm);
	wake_up_interruptible(&devp->read_queue);
	wake_up(&devp->poll_queue);

	/* Riabilita le interrupt*/
	APE_GPIOK_clearISR(devp,APE_INT_MASK);
	APE_GPIOK_restoreInt(devp,&int_status);

	printk(KERN_INFO "Interrupt Handler completa\n");

	return IRQ_HANDLED;
}

/**
  *	@brief	Callback probe del platform driver, invocata quando il modulo viene inserito.
  *	@param	op Puntatore a struttura platform_device cui l'oggetto APE_GPIOK_dev_t si riferisce.
  *	@retval	0 se completa con successo.
  */
static int APE_GPIOK_probe(struct platform_device *op){

	int i;
	int status;
	int irq;

	APE_GPIOK_dev_t *devp;
	struct device *dev;

	printk(KERN_INFO "APE_GPIOK_probe %d\n", num_of_devices);

    /* Alla prima chiamata di probe, bisogna inizializzare un array di device*/
    if(num_of_devices == 0){
        device_array = kmalloc(MAX_NUM_DEVICES*sizeof(APE_GPIOK_dev_t), GFP_KERNEL);
        if (!device_array)
    	{
    		printk(KERN_ERR "Kmalloc fallita per inizializzazione dell'array\n");
    		return -ENOMEM;
    	}
		printk(KERN_INFO "Kmalloc riuscita per inizializzazione dell'array\n");

		for (i=0; i < MAX_NUM_DEVICES; i++){
			device_array[i] = NULL;
		}

		/* Creazione della classe del device --------------------------------------*/
		APE_GPIOK_class = class_create(THIS_MODULE, DRIVER_NAME);
		if(!APE_GPIOK_class){
			printk(KERN_ERR "Creazione classe del device fallita\n");
			kfree(device_array);
			return -EFAULT;
		}
	    printk(KERN_INFO "Creazione classe del device riuscita\n");
    }

	/* Inizializzazione la struttura dell'i-esimo device*/
	devp = kmalloc(sizeof(APE_GPIOK_dev_t), GFP_KERNEL);
	if (!devp)
	{
		printk(KERN_ERR "Kmalloc della struttura APE_GPIOK_dev_t fallita\n");
		return -ENOMEM;
	}
	printk(KERN_INFO "Kmalloc della struttura APE_GPIOK_dev_t riuscita\n");

	/* Delega al kernel l'assegnazione dei device numbers*/
	status = alloc_chrdev_region(&devp->dev_num, 0, 1, DRIVER_NAME);
	if(status){
		printk(KERN_ERR "Assegnazione device numbers fallita\n");
		goto err_chrdev_region;
	}
    printk(KERN_INFO "<M,m>: <%d, %d>\n", MAJOR(devp->dev_num), MINOR(devp->dev_num));

	/* Inizializzazione e registrazione del device a caratteri*/
	cdev_init(&devp->cdev, &APE_GPIOK_fops);
	devp->cdev.owner = THIS_MODULE;

	if (device_create(APE_GPIOK_class, NULL, devp->dev_num ,NULL, "APE_GPIOK_%d",num_of_devices) == NULL){
		printk(KERN_ERR "Creazione del device %d fallita\n",num_of_devices);
		status = -EFAULT;
		goto err_device_create;
	}
    printk(KERN_INFO "Creazione del device %d riuscita\n",num_of_devices);

	/* Registra presso il kernel la struttura cdev precedentemente inizializzata*/
	status = cdev_add(&devp->cdev, devp->dev_num, 1);
	if(status){
		printk(KERN_ERR "Registrazione cdev fallita\n");
		goto err_cdev_add;
	}
    printk(KERN_INFO "Registrazione cdev riuscita\n");

	dev = &op->dev;

	/* Popola la struttura res del device*/
	status = of_address_to_resource(dev->of_node, 0, &devp->res);
	if (status){
		printk(KERN_ERR "Chiamata a of_address_to_resource fallita\n");
		goto err_addr;
	}
    printk(KERN_INFO "Chiamata a of_address_to_resource riuscita\n");

	/* Alloca area di memoria di dimensione size*/
	devp->size = devp->res.end - devp->res.start + 1;

	/* Richiesta area di memoria*/
	if  (!request_mem_region(devp->res.start, devp->size, DRIVER_NAME)){
		printk(KERN_ERR "Allocazione memoria fallita\n");
		status = -ENOMEM;
		goto err_req_mem;
	}
	printk(KERN_INFO "Allocazione memoria riuscita\n");

	/* Mapping: assegna indirizzi virtuali alla memoria I/O allocata*/
	devp->base_addr = ioremap(devp->res.start, devp->size);
	if (devp->base_addr == NULL) {
		printk(KERN_ERR "Mapping indirizzi virtuali fallito\n");
		status = -ENOMEM;
		goto err_ioremap;
	}
	printk(KERN_INFO "Mapping indirizzi virtuali riuscito\n");

	/* Parsing del DTB per ottenere per ottenere il numero della IRQ*/
	irq = irq_of_parse_and_map(dev->of_node, 0);
	devp->irq_number=irq;

	/* Registrazione dell'IRQ handler, usa le fast interrupt*/
	status= request_irq(irq,(irq_handler_t) APE_GPIOK_handler, 0, DRIVER_NAME, NULL);
	if(status){
		printk(KERN_ERR "Registrazione linea interrupt fallita %d\n",irq);
		goto err_req_int;
	}
    printk(KERN_INFO "Registrazione linea interrupt riuscita %d\n",irq);

	/* Inizializzazione delle strutture ---------------------------------------*/

	/* Associa la struttura platform device al device che si sta inizializzando*/
	devp->op = op;

	/* Wait queues*/
	init_waitqueue_head(&devp->read_queue);
	init_waitqueue_head(&devp->poll_queue);

	/* Spinlocks*/
	spin_lock_init(&devp->read_flag_sl);
	spin_lock_init(&devp->num_interrupts_sl);

	devp->read_flag = 0;
	devp->num_interrupts = 0;

	/* Aggiunge il device all'array*/
	device_array[num_of_devices] = devp;

	/* Incrementa il numero di device*/
    num_of_devices = num_of_devices + 1;

	printk(KERN_INFO "APE_GPIOK_probe %d terminata\n",num_of_devices-1);

	return 0;

	/* Gestione errori --------------------------------------------------------*/
	    free_irq(irq, NULL);
	err_req_int:
	    iounmap(devp->base_addr);
	err_ioremap:
	    release_mem_region(devp->res.start, devp->size);
	err_req_mem:
	    /* Solo of_address_to_resource*/
	err_addr:
	    cdev_del(&devp->cdev);
	err_cdev_add:
	    device_destroy(APE_GPIOK_class, devp->dev_num);
	err_device_create:
	    unregister_chrdev_region(devp->dev_num, 1);
	err_chrdev_region:
	    kfree(devp);

	return status;
}

/**
  *	@brief	Callback remove del platform driver
  *	@param	op Puntatore a struttura platform_device cui l'oggetto APE_GPIOK_dev_t si riferisce.
  *	@retval	0
  */
static int APE_GPIOK_remove(struct platform_device *op){

	int i;
	APE_GPIOK_dev_t *devp;

	printk(KERN_INFO "APE_GPIOK_remove %d iniziata\n",num_of_devices);

	/* Trova la struttura del device da rimuovere*/
	for(i = 0;i < MAX_NUM_DEVICES; i++){
		if(device_array[i]->op == op){
			devp = device_array [i];
			break;
		}
	}

	/* Rilascia le linee di interrupt*/
	free_irq(devp->irq_number, NULL);

	/* Effettua l'unmap dello spazio di memoria I/O*/
	iounmap(devp->base_addr);
	release_mem_region(devp->res.start, devp->size);

	cdev_del(&devp->cdev);

	device_destroy(APE_GPIOK_class, devp->dev_num);

	/* Rilascia il device number*/
	unregister_chrdev_region(devp->dev_num, 1);

	/* Libera lo spazio di memoria*/
	kfree(devp);

    /* Alla cancellazione dell'ultimo device*/
    if(num_of_devices == 0){
		class_destroy(APE_GPIOK_class);
	    kfree(device_array);
    } else{
        /* Decrementa il numero di device*/
        num_of_devices = num_of_devices - 1;
    }

	printk(KERN_INFO "APE_GPIOK_remove terminata %d\n",num_of_devices+1);

	return  0;
}

/**
  * @brief	Struttura di definizione delle callback probe e remove.
  */
static struct platform_driver APE_GPIOK_driver = {
		.probe = APE_GPIOK_probe,
		.remove = APE_GPIOK_remove,
		.driver = {
				.name = DRIVER_NAME,
				.owner = THIS_MODULE,
				.of_match_table = APE_GPIOK_match,
		},
};

/**
  * @brief	Macro di definizione delle callback probe e remove. Implementa a sua
  *			volta le funzioni module_init() e module_close().
  */
module_platform_driver(APE_GPIOK_driver);

/* Informazioni Modulo --------------------------------------------------------*/
MODULE_AUTHOR("Alfonso,Pierluigi,Erasmo (APE)");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION(DRIVER_NAME "APE_GPIOK");
MODULE_ALIAS(DRIVER_NAME);
/**@}*/
/**@}*/
