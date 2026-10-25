# Procesos

Cada terminal con el programa corriendo es un proceso, y cada proceso tiene sus propias variables. Si P1 cambia su `page_table`, la de P2 no cambia.

Se corre uno en cada terminal:

```
./my_dsm 1
./my_dsm 2
```

Si P2 corre en otra máquina, se le pasa la IP de P1 como segundo argumento: `./my_dsm 2 192.168.1.10`. Por defecto usa `127.0.0.1`.

El número llega a `main` por `argv[1]`. `argc` es la cantidad de argumentos y `argv` son los argumentos como texto. Con `atoi` se pasa el texto a número y ese es el `process_id`.

`init_page_table(process_id)` llena la tabla según si es P1 o P2.
