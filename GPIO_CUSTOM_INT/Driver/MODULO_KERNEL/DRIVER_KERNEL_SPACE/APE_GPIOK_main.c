/**
  ******************************************************************************
  * @file    APE_GPIOK_main.c
  * @author  Alfonso,Pierluigi,Erasmo (APE)
  * @version V1.0
  * @date    08-Luglio-2017
  *	@addtogroup DRIVER
  * @{
  * @addtogroup KERNEL
  * @{
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
#include <linux/cdev.h> /* necessario per la cdev_init */
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
#include <linux/of_irq.h>
#include <linux/interrupt.h>
#include <linux/device.h>

#include "APE_GPIOK_includes.h"

/* Macro ----------------------------------------------------------------------*/
#define DRIVER_NAME     "APE_GPIOK"	/*!< Nome del driver*/
#define NUM_APE_GPIOK_BANKS 1	/*!< Numero di device numbers contigui da assegnare*/

/* Variabili Globali ----------------------------------------------------------*/
static dev_t APE_GPIOK_dev_number;	/*!< Device number del dispositivo*/
struct class *APE_GPIOK_class;

/* Prototipi delle funzioni----------------------------------------------------*/
static int APE_GPIOK_open(struct inode *, struct file *);
static int APE_GPIOK_release(struct inode *, struct file *);
static ssize_t APE_GPIOK_read(struct file *, char *, size_t , loff_t *);
static unsigned int APE_GPIOK_poll(struct file *filp, poll_table *wait);
static irqreturn_t APE_GPIOK_handler(int irq, struct pt_regs * regs);

/**
  * @brief	Struttura del device
  */
struct APE_GPIOK_dev {
	unsigned short current_pointer;	/*!< TODO*/
	unsigned int size;				/*!< Dimensione della memoria da associare al device*/
	struct resource res;			/*!< TODO*/

	struct cdev cdev;				/*!< Struttura char device interna al kernel*/
	char name[20];					/*!< Nome del driver (DRIVER_NAME)*/
	unsigned long *base_addr;		/*!< Indirizzo base*/

	int irq_number;					/*!< Numero della linea di interrupt*/
	wait_queue_head_t read_cond;	/*!< Variabile condition per la read*/
	wait_queue_head_t poll_cond;	/*!< Variabile condition per la poll*/

	spinlock_t iNumInterrupts_sl;	/*!< Variabile lock contatore delle interrupt*/
	spinlock_t read_flag_sl;		/*!< Variabile lock per flag abilitazione lettura*/

	int iNumInterrupts;				/*!< Contatore delle interruzioni avvenute*/
	int read_flag;					/*!< Flag di abilitazione lettura*/

} *APE_GPIOK_devp;

/**
  * @brief	Stuttura delle operazioni esportate dal modulo.
  */
struct file_operations APE_GPIOK_fops = {
	.owner = THIS_MODULE,			/*!< Modulo proprietario della struttura*/
	.open = APE_GPIOK_open,			/*!< Metodo open del modulo*/
	.release = APE_GPIOK_release,	/*!< Metodo release del modulo*/
	.read = APE_GPIOK_read,			/*!< Metodo read del modulo*/
	.poll = APE_GPIOK_poll			/*!< Metodo poll del modulo*/
};

/**
  * @brief	Stuttura che identifica il device nel device-tree.
  */
struct of_device_id APE_GPIOK_match[] = {
	{.compatible = "APE_GPIOK"},	/*!< NB: La medesima stringa va copiata nel DTB*/
	{},
};

/**
  * @brief	Comunica allo spazio utente quale device è controllato dal modulo.
  */
MODULE_DEVICE_TABLE(of, APE_GPIOK_match);

/**
  * @brief	Inizializza tutte le strutture utilizzate dal driver.
  *	@param	inode: puntatore struttura inode che contiene il campo i_cdev
  *	@param	file: puntatore struttura file per accedere ai private_date
  *	@retval	0 sempre
  */
int APE_GPIOK_open(struct inode *inode, struct file *file){

	printk(KERN_INFO "APE_GPIOK_open\n");

	/* Informazioni sul device*/
	struct APE_GPIOK_dev *APE_GPIOK_devp;

	/* Tramite l'inode ricaviamo la struttura APE_GPIOK_dev che contiene
	 * a sua volta la struttura cdev.
	 */
	APE_GPIOK_devp = container_of(inode->i_cdev, struct APE_GPIOK_dev, cdev);

	/* Ottenuto il puntatore alla struttura device, lo si salva nel campo
	 * private_date della file structure per un più facile accesso.
	 */
	file->private_data = APE_GPIOK_devp;

	APE_GPIOK_devp->current_pointer = CURRENT_POINTER_OFF;

	return 0;
}

/**
  * @brief	Dealloca tutte le strutture inizializzate dalla open.
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
  *	@param	buf: puntatore al buffer user-space in cui trasferire i dati letti
  *	@param	count: lunghezza del trasferimento richiesto
  *	@retval	1 sempre
  */
ssize_t APE_GPIOK_read(struct file *file, char *buf, size_t count, loff_t *ppos){

	loff_t offset;
	struct APE_GPIOK_dev *APE_GPIOK_devp;
	unsigned char value = 0; /* Valore letto dalla periferica*/
	unsigned long *read_addr;

	printk(KERN_INFO "APE_GPIOK_read\n");

	APE_GPIOK_devp = file->private_data;
	offset = APE_GPIOK_devp->current_pointer;
	read_addr = (APE_GPIOK_devp->base_addr) + offset;

	/* Effettua la lettura in base alla modalità definita in f_flags*/
	if((file->f_flags & O_NONBLOCK) == 0){
		/* Modalità di lettura BLOCCANTE*/
		printk(KERN_INFO "Lettura bloccante\n");

		/* Effettua la wait sulla variabile condition*/
		printk(KERN_INFO "Il processo %i (%s) viene bloccato\n",current->pid, current->comm);
		wait_event_interruptible(APE_GPIOK_devp->read_cond, (APE_GPIOK_devp->read_flag & EN_BLOC_READ)==1);
		printk(KERN_INFO "Il processo %i (%s) viene risvegliato\n",current->pid, current->comm);

		/* Acquisisci il lock*/
		spin_lock(&APE_GPIOK_devp->read_flag_sl);

		/* Resetta il flag di abilitazione alla lettura*/
		APE_GPIOK_devp->read_flag &= ~(EN_BLOC_READ);

		/* Rilascia il lock*/
		spin_unlock(&APE_GPIOK_devp->read_flag_sl);

	} else {
		/* Modalità di lettura NON BLOCCANTE*/
		printk(KERN_INFO "Lettura non bloccante\n");

		/* Acquisisce il lock*/
		spin_lock(&APE_GPIOK_devp->read_flag_sl);

		/* Resetta il flag di abilitazione alla lettura*/
		APE_GPIOK_devp->read_flag &= ~(EN_NONBLOC_READ);

		/* Rilascia il lock*/
		spin_unlock(&APE_GPIOK_devp->read_flag_sl);
	}

	/* Completa la lettura del dato*/
	//TODO ioread32 prova
	value = ioread8(read_addr);
	printk(KERN_INFO "Valore Letto: %u\n",value);

	/* Incrementa la posizione*/
	*ppos = *ppos + 1;

	/* Trasferisce i date allo spazio utente*/
	copy_to_user(buf, &value, 1);

	return 1;//TODO migliorare la logica
}

/**
  *	@brief	Verifica se un'operazione di read o write su uno o più file
  *			descriptor sarà bloccante. Questa funzione deve:
  *			-Chiamare poll_wait su una o più code;
  *			-Restituire una maschera che indichi le operazioni che
  *			possono essere eseguite immediatamente senza essere bloccate.
  *	@param	file: puntatore alla struttura file
  *	@param	wait: puntatore poll_table, caricata assieme ad ogni coda che possa
  			risvegliare il processo
  *	@retval	mask: maschera delle operazioni eseguibili immediatamente
  */
unsigned int APE_GPIOK_poll(struct file *file, poll_table *wait){

	printk(KERN_INFO "APE_GPIOK_poll\n");

	struct APE_GPIOK_dev *devp = file->private_data;
	unsigned int mask = 0;

	/* La poll_wait aggiunge una nuova coda di attesa alla poll_table*/
	printk(KERN_INFO "Il processo %i (%s) viene bloccato\n",current->pid, current->comm);
	poll_wait(file, &devp->poll_cond,  wait);
	printk(KERN_INFO "Il processo %i (%s) viene risvegliato\n",current->pid, current->comm);

	/* Acquisisce il lock*/
	spin_lock(&devp->read_flag_sl);

	/* Verifica se il dato è pronto e setta la maschera*/
	if(devp->read_flag & EN_BLOC_READ){
		/* Readable device*/
		mask = POLLIN | POLLRDNORM;
	}

	/* Rilascia il lock*/
	spin_unlock(&devp->read_flag_sl);

	return mask;
}

static int APE_GPIOK_Enable(unsigned long mask){
	struct APE_GPIOK_dev *devp = APE_GPIOK_devp;
	unsigned long *addr;

	addr = devp -> base_addr + (APE_DIR_REG/4);
	iowrite32(mask, addr);

	return 1;
}

static int APE_GPIOK_Interrupt(unsigned long mask){
	struct APE_GPIOK_dev *devp = APE_GPIOK_devp;
	unsigned long *addr;

	addr = devp -> base_addr + (APE_IERR_REG/4);
	iowrite32(mask, addr);

	addr = devp -> base_addr + (APE_IERF_REG/4);
	iowrite32(mask, addr);

	return 1;
}

static int APE_GPIOK_Ack_Interrupt(unsigned long mask){
	struct APE_GPIOK_dev *devp = APE_GPIOK_devp;
	unsigned long *addr;

	addr = devp -> base_addr + (APE_ICRISR_REG/4);
	iowrite32(mask, addr);

	return 1;
}

/**
  *	@brief	ISR della periferica, le interrupt sono disabilitate durante la sua
  *			esecuzione (fast interrupt handler).
  *	@param	irq: interrupt number
  *	@param	regs: contiene uno snapshot del contesto del processore prima della ISR
  *	@retval	Valore che indica se c'è stata effettivamente una interrupt da servire,
  			in caso affermativo deve valere IRQ_HANDLED altrimenti IRQ_NONE
  */
static irqreturn_t APE_GPIOK_handler(int irq, struct pt_regs * regs){

	unsigned long flags;

	/* Disabilita le interruzioni*/
	APE_GPIOK_Interrupt(0x0);
	printk(KERN_INFO "ISR in esecuzione\n");

	/* Accesso al flag
	abilitazione alla lettura ------------------------------*/

	/* Acquisisce il lock e disabilita le interrupt, lo stato è salvato in flags*/
	spin_lock_irqsave(&APE_GPIOK_devp->read_flag_sl, flags);

	/* Segnala che si può fare una lettura (sia sincrona che asincrona)*/
	APE_GPIOK_devp->read_flag = EN_BLOC_READ | EN_NONBLOC_READ;

	/* Rilascia il lock, riabilita le interrupt e rispristina lo stato*/
	spin_unlock_irqrestore(&APE_GPIOK_devp->read_flag_sl, flags);

	/* Accesso al contatore delle interrupt avvenute --------------------------*/

	/* Acquisisce il lock e disabilita le interrupt, lo stato è salvato in flags*/
	spin_lock_irqsave(&APE_GPIOK_devp->iNumInterrupts_sl, flags);

	/* Incrementa il contatore delle interruzioni avvenute*/
	APE_GPIOK_devp->iNumInterrupts = APE_GPIOK_devp->iNumInterrupts+1;

	/* Rilascia il lock, riabilita le interrupt e rispristina lo stato*/
	spin_unlock_irqrestore(&APE_GPIOK_devp->iNumInterrupts_sl, flags);

	/* Risveglio processi -----------------------------------------------------*/
	printk(KERN_DEBUG "Processo %i (%s) risveglia i lettori...\n",current->pid, current->comm);
	wake_up_interruptible(&APE_GPIOK_devp->read_cond);
	wake_up(&APE_GPIOK_devp->poll_cond);

	/* Riabilita le interrupt*/
	APE_GPIOK_Ack_Interrupt(0xF);
	APE_GPIOK_Interrupt(0xF);

	printk(KERN_INFO "Interrupt Handler completa\n");

	return IRQ_HANDLED;
}

/**
  *	@brief	Callback probe del platform driver
  *	@param	TODO
  *	@retval	TODO
  */
static int APE_GPIOK_probe(struct platform_device *op){

	int rc;
	int irq;
	resource_size_t remap_size, phys_addr;

	printk(KERN_INFO "APE_GPIOK_probe\n");

	APE_GPIOK_devp = kmalloc(sizeof(struct APE_GPIOK_dev), GFP_KERNEL);
	if (!APE_GPIOK_devp)
	{
		printk(KERN_INFO "Kmalloc fallita\n");
		return -ENOMEM;
	}

	/* Inizializzazione e registrazione del device a caratteri*/
	cdev_init(&APE_GPIOK_devp->cdev, &APE_GPIOK_fops);
	APE_GPIOK_devp->cdev.owner = THIS_MODULE;

	/* Delega al kernel l'assegnazione dei device numbers*/
	rc = alloc_chrdev_region(&APE_GPIOK_dev_number, 0, NUM_APE_GPIOK_BANKS, DRIVER_NAME);
	if(rc){
		printk(KERN_INFO "Assegnazione device numbers fallita\n");
		goto out_bof_maj;
	}

	/* Registra presso il kernel la struttura cdev precedentemente inizializzata*/
	rc = cdev_add(&APE_GPIOK_devp->cdev, APE_GPIOK_dev_number, 1);
	if(rc)
	{
		printk(KERN_INFO "Registrazione cdev fallita\n");
		goto out_bof_cdev;
	}

	/* Creazione della classe del device --------------------------------------*/
	APE_GPIOK_class = class_create(THIS_MODULE, DRIVER_NAME);
	if(!APE_GPIOK_class){
		printk(KERN_INFO "Creazione classe del device fallita\n");
		rc = -EFAULT;
		goto out_bof_class;
	}

	if (device_create(APE_GPIOK_class, NULL, APE_GPIOK_dev_number ,NULL, "APE_GPIOK") == NULL){
		printk(KERN_INFO "Creazione del device fallita\n");
		rc = -EFAULT;
		goto out_bof_dcrt;
	}

	/* Richiesta e registrazione delle IRQ ------------------------------------*/
	struct device *dev = &op->dev;

	/* Parsing del DTB per ottenre per ottenere il numero della IRQ*/
	irq = irq_of_parse_and_map(dev->of_node, 0);

	printk(KERN_INFO "Assegnazione della linea di interruzione %d\n",irq);
	APE_GPIOK_devp->irq_number=irq;

	/* Registrazione dell'IRQ handler, usa le fast interrupt*/
	rc= request_irq(irq,(irq_handler_t) APE_GPIOK_handler, 0, DRIVER_NAME, NULL);
	if(rc){
		printk(KERN_INFO "Registrazione linea interrupt fallita %d\n",irq);
		goto out_bof_int;
	}

	rc = of_address_to_resource(dev->of_node, 0, &APE_GPIOK_devp->res);
	if (rc){
		printk(KERN_INFO "Impossibile ottenere la risorsa device\n");
		goto out_bof_addr;
	}

	/* Allocazione memoria I/O ------------------------------------------------*/

	/* Alloca area di memoria di dimensione size*/
	APE_GPIOK_devp->size = APE_GPIOK_devp->res.end - APE_GPIOK_devp->res.start + 1;

	/* Richiesta area di memoria*/
	if  (!request_mem_region(APE_GPIOK_devp->res.start, APE_GPIOK_devp->size, DRIVER_NAME)){
		printk(KERN_INFO "Allocazione memoria fallita\n");
		rc = -ENOMEM;
		goto out_bof_nomem;
	}
	printk(KERN_INFO "Allocazione memoria riuscita\n");

	phys_addr = APE_GPIOK_devp->res.start;
	remap_size = resource_size(&APE_GPIOK_devp->res);

	/* Mapping: assegna indirizzi virtuali alla memoria I/O allocata*/
	APE_GPIOK_devp->base_addr = ioremap(APE_GPIOK_devp->res.start, APE_GPIOK_devp->size);
	if (APE_GPIOK_devp->base_addr == NULL) {
		printk(KERN_INFO "Mapping indirizzi virtuali fallito\n");
		rc = -ENOMEM;
		goto out_bof_remap;
	}
	printk(KERN_INFO "Mapping indirizzi virtuali riuscito\n");

	/* Inizializzazione delle strutture ---------------------------------------*/

	/* Wait queues*/
	init_waitqueue_head(&APE_GPIOK_devp->read_cond);
	init_waitqueue_head(&APE_GPIOK_devp->poll_cond);

	/* Spinlocks*/
	spin_lock_init(&APE_GPIOK_devp->read_flag_sl);
	spin_lock_init(&APE_GPIOK_devp->iNumInterrupts_sl);

	APE_GPIOK_devp->read_flag = 0;
	APE_GPIOK_devp->iNumInterrupts = 0;

	/* Interruzioni periferica*/
	APE_GPIOK_Enable(0xF);//TODO per adesso lo piazzo qua
	APE_GPIOK_Interrupt(0xF);

	printk(KERN_INFO "Inizializzazione driver completa\n");

	return 0;

	/* Gestione errori --------------------------------------------------------*/
	out_bof_remap:
		release_mem_region(APE_GPIOK_devp->res.start, APE_GPIOK_devp->size);
	out_bof_nomem:

	out_bof_addr:
		free_irq(irq, NULL);
	out_bof_int:
		device_destroy(APE_GPIOK_class, APE_GPIOK_dev_number);
	out_bof_dcrt:
		cdev_del(&APE_GPIOK_devp->cdev);
	out_bof_cdev:
		class_destroy(APE_GPIOK_class);
	out_bof_class:
		unregister_chrdev_region(APE_GPIOK_dev_number, NUM_APE_GPIOK_BANKS);
	out_bof_maj:
		kfree(APE_GPIOK_devp);
		return rc;
}

/**
  *	@brief	Callback remove del platform driver
  *	@param	TODO
  *	@retval	TODO
  */
static int APE_GPIOK_remove(struct platform_device *op){
	printk(KERN_INFO "APE_GPIOK_remove\n");

	/* Disabilita le interrupt della periferica*/
	APE_GPIOK_Interrupt(0x0);

	resource_size_t remap_size, phys_addr;
	phys_addr = APE_GPIOK_devp->res.start;
	remap_size = resource_size(&APE_GPIOK_devp->res);

	/* Effettua l'unmap dello spazio di memoria I/O*/
	iounmap(APE_GPIOK_devp->base_addr);
	release_mem_region(APE_GPIOK_devp->res.start, APE_GPIOK_devp->size);

	/* Rilascia le linee di interrupt*/
	free_irq(APE_GPIOK_devp->irq_number, NULL);

	/* Dealloca la classe*/
	device_destroy(APE_GPIOK_class, APE_GPIOK_dev_number);
	class_destroy(APE_GPIOK_class);

	/* Rimuove il device dal sistema*/
	cdev_del(&APE_GPIOK_devp->cdev);
	unregister_chrdev_region(APE_GPIOK_dev_number, NUM_APE_GPIOK_BANKS);
	kfree(APE_GPIOK_devp);

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
