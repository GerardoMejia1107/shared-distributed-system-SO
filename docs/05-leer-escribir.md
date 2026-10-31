# Leer y escribir

Las dos funciones validan los índices y revisan la tabla antes de tocar `memory`, siempre en este orden: primero `valid` y después `access`.

## leer(page, offset, out)

1. Si `page` u `offset` se salen de rango, devuelve -1.
2. Si `valid` es 0 o `access` es menor que 1, hay fallo de página: se le pide la página al dueño y se instala localmente.
3. Deja `memory[page][offset]` en `*out` y devuelve 0.

## escribir(page, offset, value)

1. Si `page` u `offset` se salen de rango, devuelve -1.
2. Si `valid` es 0 o `access` no es 2, hay fallo de página: se consigue la propiedad exclusiva.
3. Guarda `value` en `memory[page][offset]` y devuelve 0.

`leer` ya no devuelve el byte directamente, lo deja en `out`. Así un error (-1) no se confunde con un byte que vale 0.

## Fallo de página (`handle_page_fault` en dsm.c)

| Quiero | Soy dueño | Qué pasa |
|---|---|---|
| leer | no | mando `PEDIR_LECTURA`, recibo la página y queda en solo lectura |
| escribir | no, sin copia | mando `PEDIR_ESCRITURA`, recibo la página, `access=2` y `owner` pasa a ser yo |
| escribir | no, pero tengo copia de solo lectura | mando `INVALIDAR`, espero `ACK_INVALIDAR`, `access=2` y `owner` pasa a ser yo |
| escribir | sí (con copia ajena) | mando `INVALIDAR`, espero `ACK_INVALIDAR` y `access=2` |

Después del fallo, el acceso se hace como si nada hubiera pasado.
