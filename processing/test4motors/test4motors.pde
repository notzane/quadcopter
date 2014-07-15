import processing.serial.*;

Serial quadport;

int power[] = {40,40,40,40};
int active = 4;

PFont f;

void setup() {
  size(630,300);
  rectMode(CORNER);
  f = createFont("Monospaced",16,true);
  textFont(f,16);
  background(150);
  noStroke();
  quadport = new Serial(this, "/dev/tty.usbserial-A501B6YR", 9600);
}
// xbee- /dev/tty.usbserial-A501B6YR


void draw() {
  fill(150);
  rect(0,0,630,30);
  fill(0);
  //grey rectangles
  rect(30,30,120,240);
  rect(180,30,120,240);
  rect(330,30,120,240);
  rect(480,30,120,240);
  //words
  for(int i = 0; i < 4; i++){
    text("motor "+Integer.toString(i)+" = "+power[i],30+150*i ,20);
  }
  //red rectangles
  fill(255,0,0);
  for(int j = 0; j < 4; j++){
    rect(30+150*j,270-power[j],120,power[j]);
  }
println(send(power));
//quadport.write(send(power));
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
  else if(key==CODED & active!=4) {
    if(keyCode==UP)
    power[active]++;
    else if(keyCode==DOWN)
    power[active]--;
    quadport.write(send(power));
  }
  else if(key==CODED & active==4) {
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
  quadport.write(send(power));
  }
}
