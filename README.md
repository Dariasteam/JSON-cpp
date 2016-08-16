# JSON-cpp
Permite la lectura y manipulación de ficheros **.json** en c++ de forma simple.

<!-- TOC depthFrom:2 depthTo:7 withLinks:1 updateOnSave:1 orderedList:0 -->

- [Funcionamiento Interno](#funcionamiento-interno)
- [Interfaz Provista](#interfaz-provista)
	- [Objeto Parser](#objeto-parser)
		- [Constructor](#constructor)
		- [Métodos](#m-todos)
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
  bool parseFile (string path_to_file);
```
Trata de abrir y parsear el fichero especificado en la `ruta path_to_file`.
Retorna el éxito o fracaso de la operación.
```c++
  JsonTree getTree ();
```
Retorna el objeto **JsonTree** resultante del parseo del fichero si este ha sido exitoso.

```c++
	const vector<JsonError>& getErrors ()
```
Retorna un vector con los errores encontrados durante el parseo, los cuales son de la siguiente forma

```c++
struct JsonError {
	string path;
	int flag;
};
```

Siendo los posibles flags:
#### Flags

- **NO_CLOSED**: El elemento parseado no es seguido ni por una coma ni por un símbolo de cierre,
aparentemente es el último de su colección pero esta no está convenientemente cerrada

- **EXPECTED_MORE**: El elemento parseado es seguido por la pareja coma y símbolo de cierre,
deberían existir más elementos en la colección pero esta termina abruptamente

- **EMPTY**: La colección no contiene ningún elemento

- **INVALID_KEY**: La sintaxis de la clave no se corresponde con la que exige su
colección o se ha repetido una clave (solo para hashes)

### Objeto JsonTree
#### Constructor
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

  - Copia : Se provee una interfaz homogénea para la inicialización rápida de objetos con los valores almacenados en el json. Las funciones reciben una referencia al objeto a inicializar y la ruta de la que se obtendrá el valor. Retorna el éxito o fracaso de la operación

  ```c++
	// FINAL
	bool copy (bool& to, string path);
  bool copy (string& to, string path);
  bool copy (double& to, string path);
  bool copy (float& to, string path);
  bool copy (int& to, string path);
	// VECTOR
	bool copy (vector<string>& array, string path);
	bool copy (vector<bool>& array, string path);
	bool copy (vector<double>& array, string path);
  ```

	En caso de que el vector definido en el .json contenga elementos de varios tipos los métodos devolverán false, por lo que es responsabilidad del programador asegurarse de que aquellos vectores que pretenda inicializar mediante estas funciones sean homogéneos en el .json.  

  - Información de contenedores
  ```c++
  vector <string> getKeys (string path);
  ```
  Retorna un vector con todas las claves contenidas en un **map** (*hash*) dada su ruta.
  ```c++
  int getSizeAt (string path);
  ```
  Retorna el tamaño de un **vector** dada su ruta.  


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
	if (tree.copy (int_var, "key_2") || error++);
	if (tree.copy (string_var, "key_1.[0]") || error++);
	cout << "error " << error << std::endl;
} else {
	parser.getErrors(); // return a vector of errors
}
```

---

## ToDo

- [ ] Interfaz para crear árbol desde un objeto
- [ ] Permitir escribir archivos
- [ ] Flag que oblige a todos los elementos de un contenedor a ser del mismo tipo
- [x] Manejo de errores desde el objeto parser (incompleto)
  - [x] Devolver ruta completa
  - [x] Devolver tipo de error
