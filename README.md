# ZYBO
Tutto quello che riguarda la zybo


CRITICA sui limiti della periferica:

nel caso in cui stiamo servendo una interrupt nella IRQHandler e viene innescata una ulteriore interrupt,
per come è gestita la logica dell' HW, si corre il rischio di non servire questa ulteriore interrupt.
Perchè la IRQHandler deve necessariamente azzerare tutto l'ISR prima di completare in modo tale da poter
innescare una nuova interrupt successivamente.
Il lato negativo di questa scelta è solo che un possibile interupt non venga servita, ma nel caso di un bottone
o uno switch basta ripetere il gesto.
Potrebbe invece diventare un problema con periferiche molto più veloci come SPI.
