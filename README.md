# C++ JSON Manager
Librería que provee capacidad de lectura y manejo de la información contenida
en un fichero .json de forma simplificada.

## Objeto Parser
### Constructor
```c++
 Parser ()
```
### Métodos
```c++
  bool parseFile (string path_to_file)
```

Trata de abrir y parsear el fichero especificado en la `ruta path_to_file`.
Retorna el éxito o fracaso de la operación.

```c++
  JsonTree getTree ()
```
Retorna el objeto `JsonTree` resultante del parseo del fichero. Su valor es `nullptr` hasta que un fichero haya sido parseado.


## Objeto JsonTree
Otorga la interfaz de acceso a la jerarquía

### Métodos




## Consideraciones
- Todos los tipos numéricos leidos son considerados como `double`


## Interfaz


## ToDo

- [ ] Interfaz para crear árbol desde un objeto
- [ ] Permitir escribir archivos
- [ ] Flag que oblige a todos los elementos de un contenedor a ser del mismo tipo
- [ ] Manejo de errores desde el objeto parser
  - [ ] Devolver ruta completa
  - [ ] Devolver tipo de error


## Licencia


## Usage Example
JSON File
```json
{
  "key_1" : ["element1", "element2"],
  "key_2" : 12,
  "key_4" : "Single string element",
  "key_5" : {
    "sub_key_1" : 20,
    "sub_key_2" : "last element"
  }
}
```
Code
```c++
Parser parser;
int error = 0;
if (parser.parseFile ("example_file.json")) {
  JsonTree tree = parser.getTree();
  if (tree.isNumber("key_2") || error++) // will be true
    std::cout << tree.getNumber ("key_2") << endl;
  if (tree.isString("key_5.sub_key_2") || error++) // will be true
    std::cout << tree.getString ("key_5.sub_key_2") << endl;
  if (tree.isNumber("key_1") || error++)
    std::cout << tree.getNumber("key_1") << endl; // will be false
  if (tree.isVector("key_1") || error++) {
    std::vector <std::string> array;
    if (tree.copyVector ("key_1", array)) {
      for (std::string aux : array) {
        std::cout << aux << endl;
      }
    }
  }
  cout << "error " << error << endl;
} else {
  // file can't be parsed
}
```
Output
```
12
last element
element1
element2
error 1
```
