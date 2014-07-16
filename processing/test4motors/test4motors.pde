import processing.serial.*;

Serial quadport;

int power[] = {40,40,40,40};
int active = 4;

String sending = send(power);

PFont f;

void setup() {
  size(630,300);
  rectMode(CORNER);
  f = createFont("Monospaced",16,true);
  textFont(f,16);
  background(150);
  noStroke();
  textAlign(CENTER);
  
//  quadport = new Serial(this, "/dev/tty.usbserial-A501B6YR", 9600);
//  quadport.write(send(power))  //send 40 right from the start
}
// xbee- /dev/tty.usbserial-A501B6YR


void draw() {
  coverwords();
  coverpower();
  displayvalues();
  powerbars();
}


String send(int mVal[]) {
  String toSend ="{";
  for(int k = 0; k < 4; k++) {
    int ones = mVal[k]%10;
    int tens = (mVal[k]%100)-ones;
    int hundreds = mVal[k]-tens-ones;
    toSend += str(hundreds/100);
    toSend += str(tens/10);
    toSend += str(ones);
    toSend += "/";
  }
  toSend += "}";
  return toSend;
}


void keyPressed() {
  if(key=='0' || key=='1' || key=='2' || key=='3' || key=='4'){
    active=Character.getNumericValue(key);
  }
  else if(key==CODED){  //arrow keys
    if(active!=4) {  //all motors
      if(keyCode==UP){power[active]++;}
      else if(keyCode==DOWN){power[active]--;}
    }
    else if(active==4) {
      if(keyCode==UP) {
        power[0]++;
        power[1]++;
        power[2]++;
        power[3]++;
      }
      else if(keyCode==DOWN) {
        power[0]--;
        power[1]--;
        power[2]--;
        power[3]--;
      }
    }
    sending = send(power);
//    quadport.write(sending);
    println(sending);
  }
}

void coverwords(){  //grey rect to cover words
  fill(150);
  rect(0,0,630,30);  //top
  rect(0,270,630,30);//bottom
}

void coverpower(){
  fill(0);
  for(int m = 0; m < 4; m++){  //black rectangles
    rect(30+150*m,30,120,240);
  }
}

void displayvalues(){
  for(int i = 0; i < 4; i++){  //motor vals
    text("motor "+Integer.toString(i)+" = "+power[i],90+150*i ,20);
  }
  fill(0,0,245);
  text("sent data: "+sending,315,290);  //sent data
}

void powerbars(){
  for(int j = 0; j < 4; j++){  //motor value rectangles
    if(j==active || active==4){fill(255,0,0);}  //active ones are red
    else{fill(155,0,0);}  //non-active ones are lighter
    rect(30+150*j,270-power[j],120,power[j]);
  }
}

