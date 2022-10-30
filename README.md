# Projecto Billar

## Clonar repositorio
Para clonar el repositorio:
`git clone https://github.com/NIU1599119/Billar_VGI.git --recursive`

### Actualizar submodulos (glfw, glm, imgui o bullet)
Si en algun momento hay un submodulo con la carpeta vacia significa que no esta cargado (pasa si no se utiliza `--recursive` en el comando anterior)  
En estos casos se utiliza el siguiente comando para actualizar y cargar los submodulos necesarios.
`git submodule update`

## Compilar
Utilizamos premake5 para generar los archivos de compilado con uno de los scripts siguientes segun nuestro sistema operativo

### Windows
Abrir consola en la carpeta base.  
Ejecutar `win_init.bat` para generar los archivos de solucion y projecto de visual studio 2022.  
Para compilar se utiliza el entorno de visual studio.
#### Nota
Cualquier edicion de las propiedades de la solucion se guardaran solo localmente y se reescribiran si se vuelven a generar los archivos de vs2022.  
No se puede ejecutar en un directorio con caracteres especiales (por ejemplo, con accentos)

### Linux
Abrir consola en la carpeta base.  
Ejecutar `unix_init.sh` para generar los Makefiles.  
Para compilar se utiliza el comando `make` en el directorio base

### Mac
Abrir consola en la carpeta base.  
Ejecutar `mac_init.sh` para generar los Makefiles.  
Para compilar se utiliza el comando `make` en el directorio base
#### Nota
No se ha probado si funciona

## Ejecutables
En la carpeta `bin` se encuentran todos los archivos ejecutables y compilaciones de librerias utilizadas.  
~~En todo caso el ejecutable **de momento** no depende de ningun archivo de manera que se puede distribuir solo el ejecutable.~~  
Desde que se ha implementado la importacion de modelos se deben poner el ejecutable y las carpetas de modelos.

# Importante

Aqui abajo esta el README del projecto original.  
Algunas cosas no coincidiran porque se han ido modificando para ser adaptadas a nuestro projecto.

-------------------------------------
-------------------------------------

# OpenGL Premake5 Boilerplate

This project a starting point for any application using GLFW, GLAD and GLM. It uses Premake5 to generate the build files. It provides the 'engine.h' header file which imports the libraries and there is a basic GLFW example in 'main.cpp'.

## Getting Started

Since this project builds all libraries from source and includes these as submodules, don't forget to clone the project recursively.

### Prerequisites
You will need premake5 to build this project. Download it [HERE](https://premake.github.io/download/html#v5).


### Building
Building the project is very easy. Use premake to generate the project files for your platform. Then use the correct build system to build the project.

#### Linux

```
premake5 gmake
make -j$(nproc)
```

#### Windows

```
premake5 vs2019
```

## Built With
- GLFW [Github](https://github.com/glfw/glfw)
- GLAD [Github](https://github.com/Dav1dde/glad)
- GLM  [Github](https://github.com/g-truc/glm)

## License

This project is licensed under the MIT License.
