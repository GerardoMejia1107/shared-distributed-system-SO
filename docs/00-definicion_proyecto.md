# Proyecto 10: Mini-Simulador de Memoria Compartida Distribuida (DSM) a nivel de página

## Conceptos Clave

Investiga el concepto de **Memoria Compartida Distribuida (DSM)**. El objetivo es simular una memoria compartida en un sistema donde las máquinas no la comparten físicamente (multicomputadora). Estudia cómo el **manejo de fallos de página** puede ser una herramienta para implementar DSM.

## Desafío de Implementación

- Crea dos procesos que se comuniquen por sockets (pueden ejecutarse en la misma máquina para simplificar). Estos simularán dos nodos de un sistema DSM.
- El espacio de direcciones compartido se representará con un gran arreglo, dividido lógicamente en páginas. Al principio, un proceso es "dueño" de todas las páginas.
- Cuando un proceso intente acceder a una página que no posee localmente, debe ocurrir lo siguiente:
  - a. El proceso se da cuenta de que no tiene la página y envía un mensaje por la red al proceso dueño, solicitándola.
  - b. El dueño recibe la petición, marca su copia como inválida y envía el contenido de la página al solicitante.
  - c. El solicitante recibe los datos, los instala localmente y continúa.
- **Parte compleja (Protocolo de consistencia):** Implementa un protocolo de consistencia simple. Si un nodo quiere **escribir** en una página, debe solicitar la propiedad exclusiva. El nodo dueño actual debe invalidar todas las demás copias (si permites múltiples lectores) antes de transferir la página y el permiso de escritura.

## Qué se espera en la Demo

1. **Migración de páginas para lectura:** Demostrar cómo una página migra del Proceso 1 al Proceso 2 cuando el Proceso 2 intenta leerla.
2. **Invalidación por escritura:** Demostrar que si el Proceso 2 quiere escribir en la página, primero debe enviar un mensaje de invalidación al Proceso 1 (si este aún tiene una copia de solo lectura), antes de poder realizar la escritura. Mostrar mensajes en pantalla que evidencien esta comunicación de control.
