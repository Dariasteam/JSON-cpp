# C++ JSON Manager
Permite la lectura y manipulación de ficheros **.json** en c++ de forma simple.

<!-- TOC depthFrom:2 depthTo:7 withLinks:1 updateOnSave:1 orderedList:0 -->

- [Funcionamiento Interno](#funcionamiento-interno)
- [Interfaz Provista](#interfaz-provista)
	- [Objeto Parser](#objeto-parser)
		- [Constructor](#constructor)
		- [Métodos](#mtodos)
	- [Objeto JsonTree](#objeto-jsontree)
		- [Constructor](#constructor)
		- [Métodos](#m-todos)
- [Usage Example](#usage-example)
- [ToDo](#todo)

<!-- /TOC -->

---

## Funcionamiento Interno
La información contenida en el fichero. json es almacenada en forma de árbol.

- La clase **AbstractObject** y sus clases hijas conforman los nodos del árbol en función del tipo de elemento a representar. Jerarquía:  
  - AbstractObject
    - ObjectContainer
      - ObjectMap : almacena pares *string* / *AbstractObject*\* (**hash**)
      - ObjectVector : almacena *AbstractObject*\* (**array**)
    - ObjectFinal
      - ObjectFinalString : representa strings
      - ObjectFinalNumber : representa tanto números (**double**) como booleanos


- La clase **Parser** permite parsear un fichero generando el árbol contenido en este.
- La clase **JsonTree** provee la interfaz de acceso y manipulación del árbol.

El acceso a los elementos se realiza a través de su ruta mediante la sintaxis
`key_1.key_2.1`, siendo este el valor numérico 5 (*índice* 1) del vector *key_2* contenido en el map *key_1* para el siguiente caso:

```json
{
  "key_1" : {
    "key_2" : [4, 5, 9]
  }
}
```

Debido a las límitaciones del lenguaje, debemos conocer a priori de qué tipo (*double*, *string*...) es el elemento al que queremos acceder a fin de usar el método *get* adecuado. En caso de tratar de acceder a un elemento con el tipo incorrecto **NO** ocurrirá un error, sino que se devolverán valores por defecto, a saber *false*, *""* y *-1*. Por esta razón es recomendable utilizar los métodos *is* para comprobar la consistencia de los datos antes de trabajar con ellos.

## Interfaz Provista

### Objeto Parser
- Constructor
```c++
 Parser ();
```
- Métodos
```c++
  bool parseFile (string path_to_file);
```
Trata de abrir y parsear el fichero especificado en la `ruta path_to_file`.
Retorna el éxito o fracaso de la operación.
```c++
  JsonTree getTree ();
```
Retorna el objeto **JsonTree** resultante del parseo del fichero.

### Objeto JsonTree
- Constructor
```c++
 JsonTree (AbstractObject* root);
```
Innecesario, un objeto JsonTree es generado automáticamente por el parser.
#### Métodos
  - Checkeo  : Retornan si un elemento de la jerarquía es de un tipo determinado dada su ruta.
  ```c++
  bool isNumber (string path);
  bool isString (string path);
  bool isBool (string path);
  bool isVector (string path); // array
  bool isMap (string path); // hash
  ```

  - Getters : Retornan el valor de un elemento de la jerarquía dada su ruta.
  ```c++
  double getNumber (string path);
  bool getBool (string path);
  string getString (string path);
  ```

  - Información de contenedores
  ```c++
  std::vector <string> getKeys (string path);
  ```
  Retorna un vector con todas las claves contenidas en un **map** (*hash*) dada su ruta.
  ```c++
  int getSizeAt (string path);
  ```
  Retorna el tamaño de un **vector** dada su ruta.  

  - Operaciones rápidas  
  Permiten inicializar vectores con todos los elementos contenidos en un objeto dada su ruta y el vector a inicializar. Retornan si la operación ha tenido éxito.   Cualquier contenido reexistente en el vector es eliminado y el tamaño es ajustado al de la cantidad de elementos a copiar.
  ```c++
  bool copyVector (string path, vector<double>& array);
  bool copyVector (string path, vector<bool>& array);
  bool copyVector (string path, vector<string>& array);
  ```
	Consideraciones:
  - Solo se permiten los vectores de tipo double, bool y string; en caso de querer inicializar un objeto de int, por ejemplo, se deberá inicializar un vector <double> y copiar posteriormente todos sus valores a otro vector <int>   
  - En caso de que el vector definido en el .json contenga elementos de varios tipos los métodos devolverán false, por lo que es responsabilidad del programador asegurarse de que aquellos vectores que pretenda inicializar mediante estas funciones sean homogéneos en el .json.  

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
    } else {
      // vector can't be copied
    }
  }
  cout << "error " << error << endl;
} else {
  // file can't be parsed or does not exist
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

---

## ToDo

- [ ] Interfaz para crear árbol desde un objeto
- [ ] Permitir escribir archivos
- [ ] Flag que oblige a todos los elementos de un contenedor a ser del mismo tipo
- [ ] Manejo de errores desde el objeto parser
  - [ ] Devolver ruta completa
  - [ ] Devolver tipo de error
