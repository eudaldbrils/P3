PAV - P3: estimación de pitch
=============================
1.Introducción
----------------------------------------------
El objectivo de esta páctica consiste en la estimación del pitch de ciertas señales de áudio para determinar si una trama de voz es sonora o sorda. Nuestro sistema consta de un preprocesado de la señal, un procesado y un postprocesado. El preprocesado consiste en aplicar un center clipping a la señal de entrada que producirá un doble efecto en la señal reultante de salida que puede ser muy útil, se introducirá una distorsión a la señal que hará que la intensidad de los armónicos de orden elevado y eliminaremos ruido. También hemos utlitzado un diezmado en el preprocesado para mejorar las prestaciones de nuestro código.
El procesado de la señal se basa en el cáclulo de la autocorrelación para estimar el pitch, en función de dos umbrales y la potencia determinamos si la señal es sonora o sorda. 
Finalmente el postprocesado de la señal consiste en un filtro de mediana que nos corregirá algunos valores de pitch.

2.Ejercicios básicos
----------------------------------------------
Complete el código de los ficheros necesarios para realizar la estimación de pitch usando el programa get_pitch.

* Complete el cálculo de la autocorrelación e inserte a continuación el código correspondiente.

 ```c++
void PitchAnalyzer::autocorrelation(const vector<float> &x, vector<float> &r) const {
   for (unsigned int l = 0; l < r.size(); ++l) {
      r[l] = 0;
      for(unsigned int n = l; n < x.size(); n++){
        r[l] += x[n]*x[n-l];
      }
      r[l] /= x.size();
    }

    if (r[0] == 0.0F) //to avoid log() and divide zero 
      r[0] = 1e-10; 
  }
```
Simplemente lo que hace esta función es calcular la autocorrelación de una señal. Le pasamos por parámetros a la función el vector x que corresponde a la señal y el vector r que vamos a llenar. Primeramente hacemos un for que nos va recorrer todo el vector r. Siempre empezamos el segundo for con la posición l de r a 0. El segundo for consiste en calcular la autocorrelación de la señal en una posisición (l), repetimos el proceso hasata que llenamos el vector r. El condicional del final sirve para asegurarnos que la potencia de la señal no puede ser 0. 

* Inserte una gŕafica donde, en un subplot, se vea con claridad la señal temporal de un segmento de unos 30 ms de un fonema sonoro y su periodo de pitch; y, en otro subplot, se vea con claridad la autocorrelación de la señal y la posición del primer máximo secundario

![imagen](https://user-images.githubusercontent.com/91128741/163594709-c1611d34-6994-411b-bd8d-793f158964da.png)



La primera imagen corresponde a la señal sonora de 30ms de longitud y la segunda imagen corresponde a la autocorrelación. Estos datos los hemos sacado rigurosamente del programa pitch_analyser.cpp. Hemos copiado en un fichero .txt los datos de una trama sonora dentro de la compute_pitch. El contador simplemente nos escoge la tercera trama sonora que detecta nuestro programa.
```c++
int cont=0;
    if (unvoiced(pot, r[1]/r[0], r[lag]/r[0])){
      return 0;
    }
    else{
      if (cont==3){
      FILE *foutput_x = fopen("resultats_x.txt", "w+");
      FILE *foutput_r = fopen("resultats_r.txt", "w+");
      for(unsigned int i =0; i<x.size(); i++){
        fprintf(foutput_x, "%f \n",x[i]);
        fprintf(foutput_r, "%f \n",r[i]);      
      }
      fclose(foutput_x);
      fclose(foutput_r);
    }
      cont++;
      return (float) samplingFreq/(float) lag;
    }
  }
  ```



* Determine el mejor candidato para el periodo de pitch localizando el primer máximo secundario de la autocorrelación. Inserte a continuación el código correspondiente.
Claramente se puede observar que el mejor candidato para el periodo de pitch es el puto donde se encuentra el segundo máximo de la autocorrelación, se puede ver que el pitch es de 1/0.0045s es decir de 222.22 Hz. 
El código implementado con matlab es el siguiente:

```
clear all
resultats_x = fopen("resultats_x.txt", "r");
x = fscanf(resultats_x, "%f");
T = 1/4000
t = (0:size(x)-1)*T;

r = xcorr(x);
autocorr = r(120:size(r));

subplot(2,1,1);
plot(t,x)

subplot(2,1,2);
plot(t, autocorr)
```
* Implemente la regla de decisión sonoro o sordo e inserte el código correspondiente.
La regla que hemos implementado es la siguiente:
```c++

   bool PitchAnalyzer::unvoiced(float pot, float r1norm, float rmaxnorm) const {
    if(rmaxnorm>umaxnorm && r1norm>llindarUnvoiced && pot>-43.5){
      return false;
    }
    return true;
  }
```
Lo que hacemos básicamente consiste en establecer 2 umbrales óptimos que nos permitirán la mejor estimación de las tramas sonoras o sordas posibles y nos aprovechamos de la potencia para delimitar el ruido. r1norm es un parámetro que nos indica el valor de la autocorrelación entre dos valores seguidos (r[1]/r[0]) este valor si es muy similar a 1 significará que es una trama sonora. El parámetro rmaxnorm nos indica r[l]/r[0] que significa que si este valor es similar a 1 es una trama sonora (si la autocorrelación es similar a la autocorrelación desplazada quiere decir que es una trama periódica y por lo tanto sonora). 

* Una vez completados los puntos anteriores, dispondrá de una primera versión del estimador de pitch. El resto del trabajo consiste, básicamente, en obtener las mejores prestaciones posibles con él.

    Utilice el programa wavesurfer para analizar las condiciones apropiadas para determinar si un segmento es sonoro o sordo.
    Inserte una gráfica con la estimación de pitch incorporada a wavesurfer y, junto a ella, los principales candidatos para determinar la sonoridad de la voz: el nivel de potencia de la señal (r[0]), la autocorrelación normalizada de uno (r1norm = r[1] / r[0]) y el valor de la autocorrelación en su máximo secundario (rmaxnorm = r[lag] / r[0]).
    
    ![imagen](https://user-images.githubusercontent.com/91128741/163722546-408268e8-b62d-49a8-8eb7-583cbb477b0f.png)

    Como podemos ver, los principales candidatos para determinar la sonoridad la voz son capazes de clasificar correctamente el pitch. Vemos que cuando hay algun valle en cualquiera de los candidatos, no aparece ningun pitch, eso es bueno ya que significa que es una trama sorda y por lo tanto no debe haber ningun valor elevado ni en la potencia ni en la autocorrelación ni en la autocorrelación del máximo secundario.
    

Puede considerar, también, la conveniencia de usar la tasa de cruces por cero.


    Recuerde configurar los paneles de datos para que el desplazamiento de ventana sea el adecuado, que en esta práctica es de 15 ms.
    
    Use el estimador de pitch implementado en el programa wavesurfer en una señal de prueba y compare su resultado con el obtenido por la mejor versión de su propio sistema. Inserte una gráfica ilustrativa del resultado de ambos estimadores
    ![imagen](https://user-images.githubusercontent.com/91128741/163722933-900997ee-2966-45e1-b164-178cdc5f32cc.png)

Se puede observar que el estimador de pitch de nuestro programa (la captura de arriba) es muy similar al pitch estimado por el progrma (la caputra de abajo).     
    Aunque puede usar el propio Wavesurfer para obtener la representación, se valorará el uso de alternativas de mayor calidad (particularmente Python).
    
    Optimice los parámetros de su sistema de estimación de pitch e inserte una tabla con las tasas de error y el score TOTAL proporcionados por pitch_evaluate en la evaluación de la base de datos pitch_db/train..

3.Ampliación
----------------------------------------------

- Usando la librería `docopt_cpp`, modifique el fichero `get_pitch.cpp` para incorporar los parámetros del
  estimador a los argumentos de la línea de comandos.
  
  Esta técnica le resultará especialmente útil para optimizar los parámetros del estimador. Recuerde que
  una parte importante de la evaluación recaerá en el resultado obtenido en la estimación de pitch en la
  base de datos.

  * Inserte un *pantallazo* en el que se vea el mensaje de ayuda del programa y un ejemplo de utilización
    con los argumentos añadidos.
    
    ![imagen](https://user-images.githubusercontent.com/91128741/163667427-9714fd10-bd1f-47aa-96d3-d7b59dd126ab.png)
    
    Como vemos hemos añadido tres parámetros que vamos a utlizar como umbrales. El -m umaxnorm es el umbral que nos decidirá si la autocorrelación de r[1]/r[0] es suficientemente grande para considerar la trama que estamos procesando como sonora. Los parámetros -p (llindarPos) y -n (llindarNeg) corresponden a los umbrales que vamos a utlitzar cuando realizemos el centerclipping. Finalmente el último umbral es el -u (llindarUnvoiced), este umbral nos indica si la autocorrelación de r[l]/r[0] es suficientemente mayor para poder determinar que la señal es períodica y por lo tanto sonora.
     
 
* Implemente las técnicas que considere oportunas para optimizar las prestaciones del sistema de estimación
  de pitch.

  Entre las posibles mejoras, puede escoger una o más de las siguientes:

  * Técnicas de preprocesado: filtrado paso bajo, diezmado, *center clipping*, etc.
  * Técnicas de postprocesado: filtro de mediana, *dynamic time warping*, etc.
  * Métodos alternativos a la autocorrelación: procesado cepstral, *average magnitude difference function*
    (AMDF), etc.
  * Optimización **demostrable** de los parámetros que gobiernan el estimador, en concreto, de los que
    gobiernan la decisión sonoro/sordo.
  * Cualquier otra técnica que se le pueda ocurrir o encuentre en la literatura.

Empezaremos con las técnicas de preprocesado. Nosotros primeramente hemos decidido que sería buena idea diezmar la señal para disminuir el tiempo de computación del proceso de estimación del pitch de cada fichero .wav y seguidamente evaluar su resultado. El objectivo ha sido diminuir el número de muestras temporales de los ficheros .wav para que los cálculos hechos a partir de estos ficheros sean más rápidos. Así pues, el obejetivo ha sido disminuir la frecuencia de muestreo pasando de 20KHz de los ficheros originales a 4KHz de los ficheros delmados. 
Para hacerlo, hemos utlitzado la herramienta SOX. Hemos creado una nueva carpeta anomenada pitchc_4k/train dentro de la cual se guardan los ficheros .wav delmados. Dentro de esa carpeta también se han copiado los ficheros .f0ref y también se guardaran los ficheros .f0 resultantes de la aplicación del algoritmo desenvolupado en get_pitch.cpp. Esta carpeta se encuentra dentro del directorio HOME/PAV/P3.
Para aplicar el delmado se ha accedido al directorio HOME/PAV/P3/pitch_db/train, donde se han guardado los ficheros .wav con frecuencia de muestreo 20kHz. Mediante el siguiente bucle for y la herramienta SOX se ha modificado la frecuencia de muestreo de todos los ficheros .wav y el fichero resultante se ha guardado en pitch_4k/train.
````
for fwav in *.wav; do 
    sox $fwav -r 4k ../../pitch_4k/train/$fwav
done
````
Si ahora se analizan los ficheros que se encuentran en pitch_db y los que se encuentran en pitch_4k observamos que la frecuencia de muestreo se ha modificado:

![imagen](https://user-images.githubusercontent.com/91128741/163667999-de296793-7794-4f68-98ac-6ee46668e210.png)

A continuación se muestra la velocidad de computación utlitizando la base de datos de los ficheros de 4kHz es superior a la de los ficheros de 20KHz. Para hacerlo, el fichero get_pitch.cpp se ha añadido la siguiente estructura:
```c++
unsigned t0, t1;
t0=clock(); //añadido al principio
// tot el codi
t1 = clock(); //añadido al final una vez printeado l'estimación f0 a cada trama al fichero output.txt
```
```c++
//en el main
double time = (double(t1-t0)/CLOCKS_PER_SEC);
//code
os << "Execution Time: " << time << endl; //añadido al final del main justo antoes del return 0 por tal que la última línea de los ficheros .f0 indique el tiempo de ejecución del fichero get_picth.cpp
```
Analizando el fichero rl1002.f0 del a base de datos de pitch_db se obtiene un tiempo de ejecución de:

Execution Time: 0.01697

Analizando el fichero rl1002.f0 del a base de datos de pitch_4k se obtiene un tiempo de ejecución de:

Execution Time: 0.000871

Se observa que con el diezmando, el tiempo de ejecución ha disminuido practicamente una relación de 1/20.

Otro método de preprocesado que hemos utilizado en la práctica y que comprobamos que mejora las prestaciones del sistema ha sido el centerclipping. Como se ha explicado en la introducción el centerclipping es muy útil para este tipo de estimaciones. El centerclipping nos añade una distorsión a la señal que ayudará a la estimación del pitch de dos maneras distintas: 
	
* La primera, aumentará la intensidad de los harmónicos de orden más elevado y será más fácil la detección de si una trama es sonora o no. 
	
* La segunda, nuestro sistema será más robusto respecto al ruido.

Simplemente lo que hace el centerclipping es poner a 0 todos los valores que no superen un cierto umbral, los que si superan el umbral se les restará ese mismo de manera que así se adaptará la señal. Se debe observar que nuestros umbrales son óptimos, por lo tanto los umbrales del centerclipping los decide el programa mismo y por defecto no són superiores a 0.01 y -0.01. A continuación se puede ver la parte código utilizada:
```c++
 for(unsigned int k=0; k<x.size();k++){
    if(x[k]>0){
      x[k]=x[k]-llindarPos;
      if(x[k]<0){
        x[k]=0;
      }
    } else {
      x[k]=x[k]-llindarNeg;
      if(x[k]>0){
        x[k]=0;
      }
    }
  }
 ```



Para mostrar la diferencia entre una señal con centerclipping y una sin centerplipping extremos del programa un .txt con las aplitudes de las dos señales. Voy a exagerar el centerclipping para poder ver más claramente como funciona el algorítmo.

![imagen](https://user-images.githubusercontent.com/91128741/163669294-74a15480-ab16-41a0-8f67-0af048f860b7.png)


Se observa claramente que la primera señal es la original y la segunda es la misma señal pero con un centerclipping. En este caso he puesto los umbrales a 0.02 y a -0.02 para conseguir este clipping. También se ha comprobado que el preprocesado con centerclipping mejora las prestaciones:

![imagen](https://user-images.githubusercontent.com/91128741/163669387-45966418-2f0e-4fa1-aa29-6b2fe0c82f7b.png)

La primera captura pertenece a la señal sin clipping y la segunda a la señal con clipping. Se ha mejorado considerablemente el resultado aplicando centerclipping.
 
Para mejorar el resultado también hemos utlitzado un postprocesado de la señal, concretamente un filtro de mediana. El filtro de mediana consiste en evaluar la amplitud de los pitch y comparar con sus vecinos. En nuestro caso hemos creado un vector de tres muestras que llamaremos filtro de mediana. Este filtro coge 3 pitch calulados, las ordena de menor a mayor y vamos rellenando otro vector con la muestra del centro del vector filtro de mediana.
El código utlitzado es el siguiente:
```c++
  vector<float> fMediana;
  vector<float> f0_;
  int fMedianaLen=3;
  f0_.push_back(f0[0]);
  for (unsigned int l=1; l<f0.size()-1; l++){

    for(int r=-1; r<2; r++){
      fMediana.push_back(f0[l+r]);
    }
    sort(fMediana.begin(),fMediana.end());
    f0_.push_back(fMediana[1]);//generalitzar l'1
    fMediana.clear();
  }
   f0_.push_back(f0[f0.size()-1]);
```

Es importante este filtro porque nos corrige errores del procesado y nos mejora las prestaciones.

![imagen](https://user-images.githubusercontent.com/91128741/163670108-7990a494-7247-4032-a479-a405d62b74d3.png)

La primera imagen corresponde a la evaluación de la señal sin el filtro de mediana y la segunda corresponde a la señal con filtro de mediana, se puede comprobar que mejoramos las prestaciones utlitzando el filtro de mediana.

Por último mostramos como hemos optimizado los umbrales de manera que mejoren lo máximo posible las prestaciones. Para empezar hemos creado un fichero .sh que ejecutándolo entra en unos bucles que calculan para cada variable la mejor combinación entre las otras. El código utiltizado ha sido el siguiente:
```sh
for umaxnorm in $(seq 0.2 0.1 0.4);do
    for llindarNeg in $(seq -0.02 0.01 0);do
        for llindarPos in $(seq 0 0.01 0.02);do
            for llindarUnvoiced in $(seq 0 0.001 0.003);do
                echo -n "umaxnorm=$umaxnorm llindarNeg=$llindarNeg llindarPos=$llindarPos llindarUnvoiced=$llindarUnvoiced   "
                scripts/run_get_pitch.sh $umaxnorm $llindarNeg $llindarPos $llindarUnvoiced > /dev/null
                pitch_evaluate pitch_4k/train/*f0ref | fgrep TOTAL
            done  
        done

    done 
    
done
```


 











