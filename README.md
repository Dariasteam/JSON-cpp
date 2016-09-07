[![Build Status](https://travis-ci.org/Dariasteam/JSON-cpp.svg?branch=master)](https://travis-ci.org/Dariasteam/JSON-cpp)
# JSON-cpp    

Permite la lectura y manipulación de ficheros **.json** en c++ de forma simple.

<!-- TOC depthFrom:2 depthTo:6 withLinks:1 updateOnSave:1 orderedList:0 -->

- [Funcionamiento Interno](#funcionamiento-interno)
- [Interfaz Provista](#interfaz-provista)
	- [Objeto Parser](#objeto-parser)
		- [Constructor](#constructor)
		- [Métodos](#mtodos)
	- [Objeto JsonTree](#objeto-jsontree)
		- [Constructor](#constructor)
		- [Métodos](#mtodos)
			- [Lectura del árbol](#lectura-del-rbol)
			- [Escritura del árbol](#escritura-del-rbol)
- [Usage Example](#usage-example)
- [ToDo](#todo)

<!-- /TOC -->

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


- La clase **Parser** permite abrir/guardar y parsear un fichero generando el árbol del mismo.
- La clase **JsonTree** provee la interfaz de acceso y manipulación del árbol.

El acceso a los elementos se realiza a través de su ruta mediante la sintaxis
`key_1.key_2.1` ó `key_1.key_2.[1]` indistintamente, siendo este el valor numérico 5 (*índice* 1) del vector *key_2* contenido en el map *key_1* para el siguiente caso:

```json
{
  "key_1" : {
    "key_2" : [4, 5, 9]
  }
}
```

## Interfaz Provista

### Objeto Parser
#### Constructor
```c++
 Parser ();
```
#### Métodos
```c++
  int parseFile (string path_to_file, JsonTree& tree, bool verbose = true);
```
Trata de abrir y parsear el fichero especificado en la ruta `path_to_file`.
El árbol generado es guardado en el objeto `tree` y retorna uno de los siguientes
valores:

- **0x00001** JSON_PARSE_OUTPUT::OK
- **0x00010** JSON_PARSE_OUTPUT::CANT_OPEN_FILE
- **0x00100** JSON_PARSE_OUTPUT::WARNINGS						         
- **0x01000** JSON_PARSE_OUTPUT::ERRORS
- **0x10000** JSON_PARSE_OUTPUT::EMPTY_FILE  

Para los casos 3 y 4 existen

```c++
const vector<JsonLog>& getErrors ();
const vector<JsonLog>& getWarnings ();

struct JsonLog {
	string path;
	int flag;
};
```
Retorna un vector con los errores / warnings encontrados durante el parseo.

Flags:

- JSON_PARSER_FLAG::NO_CLOSED: El elemento parseado no es seguido ni por una coma ni por un símbolo de cierre,
aparentemente es el último de su colección pero esta no está convenientemente cerrada

- JSON_PARSER_FLAG::EXPECTED_MORE: El elemento parseado es seguido por la pareja coma y símbolo de cierre,
deberían existir más elementos en la colección pero esta termina abruptamente

- JSON_PARSER_FLAG::EMPTY: La colección no contiene ningún elemento

- JSON_PARSER_FLAG::INVALID_KEY: La sintaxis de la clave no se corresponde con la que exige su
colección o se ha repetido una clave (solo para hashes)

```c++
	static int saveFile (string fileName, JsonTree& tree);
```
Devuelve los mismos flags que el método parseFile. Nótese no se necesita instancia


### Objeto JsonTree

#### Constructor
```c++
 JsonTree (AbstractObject* root);
 JsonTree ();
```
#### Métodos
##### Lectura del árbol
- Checkeo  : Retornan si un elemento de la jerarquía es de un tipo determinado dada su ruta.
```c++
bool isNumber (string path);
bool isString (string path);
bool isBool (string path);
bool isVector (string path); // array
bool isMap (string path); // hash
```
- Información de contenedores
```c++
vector <string> getKeys (string path);
```
Retorna un vector con todas las claves contenidas en un **map** (*hash*) dada su ruta.
```c++
int getSizeAt (string path);
```
Retorna el tamaño de un **vector** dada su ruta.  

- Copia : Reciben una referencia al objeto a inicializar y la ruta de la que se obtendrá el valor. Retorna el éxito o fracaso de la operación.
```c++
// FINAL
bool get (bool& to, string path);
bool get (string& to, string path);
bool get (double& to, string path);
bool get (float& to, string path);
bool get (int& to, string path);
// VECTOR
bool get (vector<string>& array, string path);
bool get (vector<bool>& array, string path);
bool get (vector<double>& array, string path);
```
(En caso de que un vector definido en el .json contenga elementos de varios tipos los tres
últimos métodos devolverán false)

- Salida formateada
```c++
string toText ();
```
##### Escritura del árbol
Métodos con la siguiente forma si no se especifica lo contrario.
```c++
bool method (<type> value, string path)
```
  Dónde _type_ puede adoptar `bool, float, int, double, string, const char,
vector<double>, vector<int>, vector<bool>, vector<string>`


- Adición : Retornan true si no existe la ruta especificada y esta es creada correctamente.
Las rutas pueden ser de tamaño arbitrario, generándose la jerarquía de hashes especificada en path automáticamente
```c++
bool add (double value, string path);
...
bool add (vector<double>& array, string path);
```
Se comporta como un push_back en el caso de que la ruta sea un vector.

  También pueden crearse contenedores vacíos.
  ```c++
  bool addVector (string path);
  bool addMap (string map);
  	```

- Reemplazo : Retornan true si existe la ruta especificada y su contenido es actualizado correctamente
```c++
bool add (double value, string path);
...
bool add (vector<double>& array, string path);
```

- Adición forzada : Retorna true si la ruta especificada adopta el valor. En caso de existir
previamente su valor es sustitudio, en caso de no existir es creada.
Las rutas pueden ser de tamaño arbitrario, generándose la jerarquía de hashes especificada en path automáticamente
```c++
bool set (double value, string path);
...
bool set (vector<double>& array, string path);
```

- Borrado
```c++
bool erase (string path);
```
Retorna true si la ruta existe y es borrada correctamente
```c++
bool remove (string path);
```
Retorna true si la ruta es borrada correctamente o no existía

## Usage Example
Input
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

using namespace json;
using namespace std;

Parser parser;
JsonTree tree;
int number = 0;
vector <int> vec;
for (int i = 0; i < 5; i++)
  vec.push_back (i*4);

if (parser.parseFile ("example_file.json", tree) & JSON_PARSE_OUTPUT::OK) {
  tree.get (number, "key_5.sub_key_1");  // number = 20, return true
  tree.isNumber ("key_1");               // return false
  tree.add ("hard_coded_string","newKey.some.long.path"); // return true
  tree.add (vec, "key_5.vector");    // copy values from vec
  tree.add (true, "key_5.vector");   // pushback
	Parser::saveFile ("example_file.json", tree);
}
```
Output
```json
{
  "key_1" : [
    "element1",
    "element2"
  ],
  "key_2" : 12.000000,
  "key_4" : "Single string element",
  "key_5" : {
    "sub_key_1" : 20.000000,
    "sub_key_2" : "last element",
    "vector" : [
      0.000000,
      4.000000,
      8.000000,
      12.000000,
      16.000000,
      true
    ]
  },
  "newKey" : {
    "some" : {
      "long" : {
        "path" : "hard_coded_string"
      }
    }
  }
}
```
---

## ToDo

- [x] Interfaz para crear árbol desde un objeto
- [x] Permitir escribir archivos
- [ ] Flag que oblige a todos los elementos de un contenedor a ser del mismo tipo
- [ ] Mejorar la salida de los numeros enteros
- [ ] Reescribir README en inglés
- [ ] Resolver ambigüedad add para vectores
- [x] Crear namespaces
- [x] Manejo de errores desde el objeto parser
  - [x] Devolver ruta completa
  - [x] Devolver tipo de error
