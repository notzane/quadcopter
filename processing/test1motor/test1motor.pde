import processing.serial.*;
int throttle = 40;
Serial send;

void setup() {
  size(30,120);
  println(Serial.list());
  send = new Serial(this, "/dev/tty.usbserial-A501B6YR", 9600);
}

void draw() {
  background(0);
  fill(255);
  textAlign(CENTER);
  text(throttle, width/2, height/2);
  
  //sthrottle = height - mouseY + 40;
  if (throttle > 160) {
    throttle = 160;
  }
  else if (throttle < 40) {
    throttle = 40; 
  }
  send.write(throttle);
}

void keyPressed() {
  if (key == ' ') {
    throttle = 40;
  }
  if (key == CODED) {
    if (keyCode == UP) {
      throttle += 1;
    }
    if (keyCode == DOWN) {
      throttle -= 1;
    }
  }
}


