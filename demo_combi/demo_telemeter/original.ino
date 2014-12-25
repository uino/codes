/*===========================================================================
http://youtu.be/qUgFji4Figw
3v!!!!!!!
*/

boolean recdata = true;
boolean data;
int buf[64];
int rc=0;



int getdist(){
  int litera;
  
  if (Serial1.available() > 0){
    while (Serial1.available() > 0){
      litera = Serial1.read();
      if (litera == 42) { //Если принят знак "*"
        data = true;      //то устанавливаем признак начала пакета
      }

      if (litera == 35) { //Если принят знак "#"
        data = false;     //то устанавливаем признак окончания пакета...
        recdata = true;   //и устанавливаем признак получения данных для управления (сбросом) таймера и дальнейшей обработки пакета
      }
      if(data==true && rc<40 && litera>47){ //Если есть признак начала пакета, длина пакета в разумных пределах и litera имеет цифровое значение по ASCII, то...
        litera = litera-48;// Преобразуем ASCII в цифру...
        buf[rc] = litera;  // и добавляем ее в массив.
        rc++;
      }
    }
  }else{
    if (recdata == true){
      boolean dig=true;   //Эта переменная будет работать для разделения пакета на разряды по 2 цифры
      int countdata=0;    //Эта переменная будет считать разряды
      int data=0;         //Эта переменная будет принимать значения разрядов
      int sum=0;          //Это сумма всех разрядов за исключения последнего
      int src=0;          //Это значение последнего разряда (10), в котором прописана контрольная сумма
      int countLaser=0;   //Это значение внутреннего счетчика в 5-м разряде
      int dist=0;         //Это дистанция, которую мы подсчитаем
      for(int p = 0; p<rc; p++){
        if(dig){
          data = buf[p]*10;//Здесь мы первый знак каждого нового разряда умножаем на 10....
          countdata++;
        }else{
          data += buf[p]; //... а здесь к нему прибавляем второе значение.
          if(countdata<10)sum+=data;//здесь подсчитваем контрольную сумму
          if(countdata==5)countLaser=data;//здесь информация о счетчике
          if(countdata==7)dist=data*10000;//здесь считаем дистанцияю---------|
          if(countdata==8)dist+=data*100;//                                  |
          if(countdata==9)dist+=data;//--------------------------------------|
          if(countdata==10)src=data;//здесь извлекаем контрольную сумму из пакета
          Serial.print(data);
          Serial.print(" ");
          data=0;
        }
        buf[p]='\0';
        dig=!dig;
      }
    
      if(sum >= 100) {//Если контрольная сумма больше 99, то отсекаем лишнее, оставляя только последние два знака
        int a=sum;
        sum=sum/100;
        sum=sum*100;
        sum =a-sum;
      }
      if(sum==src){//Если сумма разрядов (за исключением последнего) равна контрольной сумме (последний разряд) то...
//        Serial.print(" ");
//        Serial.print(sum);
//        Serial.print(" ");
//        Serial.print(src);
        Serial.print("\t");
        Serial.print(countLaser); //Выводим данные внутреннего счетчика
        Serial.print("\t");
        Serial.print(dist);//Выводим дистанцию и...
        if(countLaser==99){//если счетчик достиг предельного значения, то...
         Serial1.write("*00084553#"); //даем команду на начало нового цикла
        }
      }
      src = 0;
      countdata = 0;
      countLaser = 0;
      rc=0;
      recdata = false;
      Serial.println();
    }
  }
}


void setup()
{
    Serial.begin(115200);
    Serial1.begin(115200);
    //Serial.println("Start...");
}

void loop()
{
  static unsigned long t = 0;
  if (millis() > (t + 2000)){ // Таймер запуска при отсутствии данных от датчика
    Serial1.write("*00084553#");
    t = millis();
  }
  getdist();
  if (recdata) t = millis(); // сброс таймера при получении данных от датчика
}


/*
Se il est adopté par le signe "*"
puis réglez le signe du début du paquet
Se il est adopté, le "#"
puis réglez le signe de la fin de l'emballage ...
et mettre un signe pour obtenir la gestion de données (reset) la minuterie et plus progiciel de traitement
Se il est un signe de début du paquet, la longueur du paquet, et la valeur numérique raisonnablement Litera est ASCII, alors ...
  Transformer ASCII de comprendre ...
  et ajouter à la gamme.
Cette variable se emploiera à séparer le paquet dans les catégories 2 chiffres
Cette variable sera considérée comme décharges
Cette variable prendra les valeurs de bits
Ce est la somme de tous les chiffres sauf le dernier
Cette valeur est la dernière catégorie (10), qui définit la somme de contrôle
La valeur du compteur interne dans la cinquième chiffres
Cette distance, qui nous calculons
Ici, nous avons le premier caractère de chaque nouveau chiffre est multiplié par 10 ....
... Et ici, nous ajoutons la deuxième valeur.
checksum ici podschitvaem
déclarations concernant la lutte
ici croire distantsiyayu --------- |
                                   |
-------------------------------------- |
ici tirer la somme de contrôle du paquet
Si la somme de contrôle est supérieur à 99, puis coupez l'excès, ne laissant que les deux derniers chiffres
Si la quantité de bits (sauf le dernier) est égale à la somme de contrôle (le dernier chiffre), puis ...
         Serial.print ("");
         Serial.print (somme);
         Serial.print ("");
         Serial.print (src);
Fournit en sortie les données du compteur interne
Dériver la distance et ...
Si le compteur a atteint la limite, le ...
donner la commande pour démarrer un nouveau cycle
Serial.println ("Démarrer ...");
  Lancer minuterie lorsque aucune donnée provenant du capteur
  réinitialiser la minuterie lorsqu'elle reçoit des données provenant du capteur


*/