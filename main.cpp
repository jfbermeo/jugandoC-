#include <stdio.h>
#include <stdlib.h>//srand, rand
#include <time.h>//time
#include "Constantes.h"
#include "ArrayDinamico.h"
#include "Tabla.h"
#include "Nodo.h"
#include "Ruta.h"

int random(int inicio, int final);

int main()
{
	srand (time(NULL));//la semilla para generar los n�meros aleatorios cada vez que se ingresa al random. esta funcion solo se llamana una sola vez


	//datos para llenar la tabla distancia  (nodo x nodo)
	int distancia[]={0,230,250,320,150,230,452,420,230,320,250,
		230,0,25,65,47,85,25,14,36,96,85,
		250,25,0,4,68,5,21,23,5,69,54,
		320,65,4,0,7,9,5,47,52,32,1,
		150,47,68,7,0,5,96,3,56,21,12,
		230,85,5,9,5,0,2,4,6,8,96,
		452,25,21,5,96,2,0,85,41,52,32,
		420,14,23,47,3,4,85,0,14,25,74,
		230,36,5,52,56,6,41,14,0,8,10,
		320,96,69,32,21,8,52,25,8,0,4,
		250,85,54,1,12,96,32,74,10,4,0
	};
	//datos para llenar la tabla demanda  = (nodo x dias)
	int demanda[]=  {3,7,
		8,4,
		4,5,
		10,10,
		3,5,
		4,6,
		8,4,
		3,7,
		2,2,
		1,4};

	//datos para llenar la tabla capacidad (nodo)
	int capacidad[]={10,
		10,
		10,
		10,
		10,
		10,
		10,
		10,
		10,
		10};

	//Contenedor de los datos
	ArrayDinamico<int> lstDistancia(numeroNodos+1,numeroNodos+1);//Creamos una matriz distancia de numeroNodos+1  X numeroNodos+1--    mas 1 por que estamos sumando el deposito
	ArrayDinamico<int> lstDemanda(numeroDias,numeroNodos);//Matriz de demanda 2 x numero de Nodos
	ArrayDinamico<int> lstCapacidadAlmacenamiento(1,numeroNodos); // matriz de almacenamiento de 1 x numero de Nodos


	ArrayDinamico<int> lstInventario(numeroDias,numeroNodos);//


	//Creacion de los Nodos
	Tabla<Nodo> lstNodo;

	//Llenar Valores para la Distancia
	//asignar los valores 
	int indice=0;
	for (int y = 0; y<lstDistancia.GetLengthY(); y++)
	{
		for (int x = 0; x<lstDistancia.GetLengthX(); x++)
		{

			lstDistancia.Set(x,y,distancia[indice++]);
		}
	}

	//Llenar Valores para la demanda
	//asignar los valores 
	indice=0;
	for (int y = 0; y<lstDemanda.GetLengthY(); y++)
	{
		for (int x = 0; x<lstDemanda.GetLengthX();  x++)
		{
			lstDemanda.Set(x,y,demanda[indice++]);
		}
	}

	//Llenar Valores para la capacidad
	//asignar los valores 
	indice = 0;
	for (int y = 0;y< lstCapacidadAlmacenamiento.GetLengthY(); y++)
	{
		for (int x = 0; x<lstCapacidadAlmacenamiento.GetLengthX(); x++)
		{
			lstCapacidadAlmacenamiento.Set(x,y,capacidad[y]);
		}
	}

	//Llenar Tabla Inventario
	indice = 0;

	int tmpInventarioAnterior;
	int tmpDemanda;
	int tmpCantidadRecoger;

	//recorrer todos los nodos
	for (int y = 0;y< lstInventario.GetLengthY() ; y++)
	{
		for (int x = 0; x<lstInventario.GetLengthX(); x++)
		{
			tmpInventarioAnterior = 0;
			if(x>0)//si no es el primer dia, traer el inventario anterior
			{
				tmpInventarioAnterior = lstInventario.Get(x-1,y);
			}
			tmpDemanda = lstDemanda.Get(x,y);


			//con esto evaluamos, si el rango maximo para CR es la capacidad disponible o la capacidad del vehiculo
			int disponibleInventario = lstCapacidadAlmacenamiento.Get(0,y)-tmpInventarioAnterior;
			int mayor = capacidadVehiculo;
			if(disponibleInventario<capacidadVehiculo)
			{
				mayor=disponibleInventario;
			}

			//de esta forma se controla que la operacion in = i+cd-d  // no nos vaya a dar negativo
			int diferencia =tmpInventarioAnterior- tmpDemanda;
			if(diferencia>=0)//si la diferencia en positiva,es por que la capacidad de inventario, suple la capacidad de demanda
			{
				//no necesitamos evaluar, ya que el inventario disponible, tiene la cantidad necesaria
				tmpCantidadRecoger =0;//random(0,mayor);
			}
			else//si la diferencia es negativa (hay mas en la demanda que en el inventario)
			{
				diferencia*=-1;

				if(diferencia<=mayor)//si la diferencia es menor a la capacidad del vehiculo, CR es un numero al azar entre diferencia y capacidad del vehiculo
				{
					tmpCantidadRecoger = random(diferencia,mayor);
				}
				else//si la diferencia es mayor a la capacidad del vehiculo, CR es un numero entre 0 y capacidad del vehiculo
				{
					tmpCantidadRecoger =random(0,mayor);
				}
			}

			//printf("cantidad a recoger = %d\n",tmpCantidadRecoger);

			//Inventario = inventarioAnterior + cantidadRecoger - demanda
			lstInventario.Set(x,y,tmpInventarioAnterior+tmpCantidadRecoger-tmpDemanda);
		}
	}

	//Insertar Nodos a la Tabla
	for (int i = 1; i <= numeroNodos; i++)
	{
		//i-1 por que el valor del nodo1, esta en el indice 0, del nodo2 esta en el indice 1, nodo3 en indice 2 ...
		Nodo *nodo =new Nodo(i,lstCapacidadAlmacenamiento.Get(0,i-1),i-1, lstDemanda.GetListY(i-1),lstDistancia.GetListY(i),lstInventario.GetListY(i-1));
		lstNodo.Insertar(nodo);
	}


	//Crear Tabla de Rutas
	//es una tabla que contiene una tabla de rutas.
	//Cada Tabla de Rutas, son las rutas establecidad por cada dia
	Tabla<Tabla<Ruta>> *lstRuta = Ruta::GenerarRuta(lstNodo);//Declarar variable, donde va a quedar el listado de Rutas
	//GenerarRuta(lstRuta,lstNodo);//Ruta.h GenerarRuta

	/***************************************************************
	*
	*	mostrar Mensajes
	*
	****************************************************************/

	printf("\nCapacidad de Vehiculo : %d\n", capacidadVehiculo);
	printf("\nN de Nodos : %d\n", numeroNodos);
	printf("\nN de Dias : %d\n", numeroDias);
	printf("\nMaximo Nodos por Rutas : %d\n", maxNodoRuta);

	//imprimir
	printf("\n\ndistancia\n");
	lstDistancia.Imprimir("%d");


	//imprimir Demanda
	printf("\nDemanda\n");
	lstDemanda.Imprimir("%d");

	//imprimir Capacidad Almacenamiento
	printf("\nCapacidad Almacenamiento\n");
	lstCapacidadAlmacenamiento.Imprimir("%d");

	//imprimir Demanda
	printf("\nInventario\n");
	lstInventario.Imprimir("%d");

	printf("\n\nMostrar Nodos\n");
	for (int i = 0; i < lstNodo.GetLength(); i++)
	{
		lstNodo.Get(i)->Imprimir();
	}

	printf("\n\nMostrar Rutas\n");


	int totalDistancia=0, totalDemanda=0,totalCantidadRecoger=0,totalInventario=0;//variablers calculo general de las rutas
	int tDe,tcr,tDi,ti;//variables calculo temporal
	//recorre cada uno de las tablas de rutas que se crearon
	for (int x = 0; x < lstRuta->GetLength(); x++)
	{
		Tabla<Ruta> *tablaRutaDia = lstRuta->Get(x);
		for (int i = 0; i < tablaRutaDia->GetLength(); i++)//recorremos cada ruta
		{
			tablaRutaDia->Get(i)->Imprimir();
		}

		tDi=tablaRutaDia->Totalizar(&Ruta::TotalDistancia);
		tDe=tablaRutaDia->Totalizar(&Ruta::TotalDemanda);
		tcr=tablaRutaDia->Totalizar(&Ruta::TotalCantidadRecoger);
		ti=tablaRutaDia->Totalizar(&Ruta::TotalInventario);

		totalDistancia+=tDi;
		totalDemanda+=tDe;
		totalCantidadRecoger +=tcr;
		totalInventario+=ti;

		printf("\n\nTD: %d\t\tTCR: %d\t\tTC: %d\t\tTI: %d",tDi,tcr,tDe,ti);
		//printf("\ntotal Demanda: %d",tDe);
		//printf("\ntotal Cantidad Recoger: %d",tcr);
		printf("\n\n");
	}
	printf("\n\n");
	printf("--------- Total -------------");
	printf("\ntotal Distancia: %d",totalDistancia);
	printf("\ntotal Demanda: %d",totalDemanda);
	printf("\ntotal Cantidad Recoger: %d",totalCantidadRecoger);
	printf("\ntotal Inventario: %d",totalInventario);
	printf ("\n\nTC: Total Carga \nTCR: Total Cantidad Recoger\nTD: Total Distancia\nTI: Total Inventario");

	getchar();

	return 0;
}

