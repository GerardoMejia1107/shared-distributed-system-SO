# Demo

Compilar y abrir dos terminales:

```
make
./my_dsm 1      # terminal 1 (se queda esperando a P2)
./my_dsm 2      # terminal 2
```

Comandos: `leer <pagina> <offset> [cantidad]`, `escribir <pagina> <offset> <texto>`, `tabla`, `ayuda` y `salir`.

## Demo 1: migración de páginas para lectura

```
P1> escribir 5 0 Hola DSM          (P1 es dueño, escribe sin mensajes)
P2> leer 5 0 8                     (fallo de página en P2)
```

En pantalla: P2 manda `PEDIR_LECTURA`, P1 pasa su copia a solo lectura y manda `ENVIAR_PAGINA`, P2 la instala y muestra `Hola DSM`. Con `tabla` en las dos terminales se ve la página 5 con `valid=1 access=1` en ambas.

## Demo 2: invalidación por escritura

```
P2> escribir 5 0 Chau              (P2 solo tiene lectura)
```

En pantalla: P2 manda `PEDIR_ESCRITURA`, P1 invalida su copia y cede la propiedad, P2 recibe la página con permiso de escritura. Con `tabla`: P1 queda con `valid=0 access=0 owner=P2` y P2 con `valid=1 access=2 owner=P2`.

Para ver `INVALIDAR` y `ACK_INVALIDAR`, que lo hace el dueño cuando escribe teniendo una copia ajena:

```
P1> leer 5 0 4                     (P1 pide la página a P2, quedan las dos en solo lectura)
P1> escribir 5 0 Ping              (P1 es la dueña ahora, invalida la copia de P2)
```

## Otras pruebas

- `leer 99 0` y `escribir 5 4096 x` muestran que se validan los índices.
- Cerrar un proceso hace que el otro avise y termine.
