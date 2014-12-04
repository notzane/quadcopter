import processing.serial.*;

Serial quadport;

int throttle = 0;

int forBack = 0;
char fOrB = 'f';

int lefRight = 0;
char lOrR = 'l';

void setup() {
  size(640, 320);
  textAlign(CENTER);

  quadport = new Serial(this, "/dev/tty.usbmodem1421", 9600);
}

void draw() {
  background(149);
  
  drawThrottle();
  drawDirection(lefRight, forBack);
}

String send() {
  String toSend = "{";
  int ones = throttle%10;
  int tens = throttle%100 - ones; //In case the tens digit is a 0, makes sure to add a space for it
  int hundreds = throttle - tens - ones;
  
  //Adds throttle
  toSend += str(hundreds/100);
  toSend += str(tens/10);
  toSend += str(ones);
  //Adds forward/backwards
  toSend += str(fOrB);
  toSend += str(forBack);
  //Adds left/right
  toSend += str(lOrR);
  toSend += str(lefRight);
  //Adds end
  toSend += "}";
  return toSend;
}

void drawThrottle() {
  fill(0);
  noStroke();
  rect(40, 60, 80, 200);
   
  fill(0, 128, 256);
  rect(40, 260 - throttle/3.6, 80, throttle/3.6);
  
  fill(255);
  textSize(20);
  text(throttle, 80, 240);
}

void drawDirection(int x, int y) {
  stroke(255);
  line(380, 70, 380, 250);
  line(290, 160, 470, 160);
  
  //Scale to graph
  x *= 10;
  y *= 10;
  
  if (fOrB == 'f') {
    y *= -1;
  }
  if (lOrR == 'l') {
    x *= -1;
  }
  stroke(255, 0, 0);
  strokeWeight(3);
  line(380, 160, 380 + x, 160 + y);
}

void keyPressed() {
  //Direction
  if (key == CODED) {
    if (keyCode == UP) {
      forBack++;
    }
    else if (keyCode == DOWN) {
      forBack--;
    }
    else if (keyCode == LEFT) {
      lefRight ++;
    }
    else if (keyCode == RIGHT) {
      lefRight --;
    }
  }
  else if (key == 'f') {
    fOrB = 'f';
  }
  else if (key == 'b') {
    fOrB = 'b';
  }
  else if (key == 'l') {
    lOrR = 'l';
  }
  else if (key == 'r') {
    lOrR = 'r';
  }
  //Throttle
  else if (key == 'w') {
    throttle += 1;
  }
  else if (key == 's') {
    throttle -= 1;
  }
    else if (key == 'q') {
    throttle += 10;
  }
  else if (key == 'a') {
    throttle -= 10;
  }
  //Limits range of throttle and direction
  if (forBack > 9) {forBack = 9;}
  else if (forBack < 0) {forBack = 0;}
  
  if (lefRight > 9) {lefRight = 9;}
  else if (lefRight < 0) {lefRight = 0;}
  
  if (throttle > 999) {throttle = 999;}
  else if (throttle < 0) {throttle = 0;}
  //Sends through serial when a key is pressed
  println(send());
  quadport.write(send());
}

  
