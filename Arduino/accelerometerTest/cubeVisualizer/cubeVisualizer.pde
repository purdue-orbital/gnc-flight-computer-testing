import processing.serial.*;

float pitch, roll;

Serial arduino;

void setup(){
  size(1200, 800, OPENGL);
  arduino = new Serial(this, "COM3", 9600);
  arduino.bufferUntil('\n');
}

void draw(){
  background(32);
  smooth();
  lights();
  
  fill(#FF9F03);
  noStroke();
  
  translate(width/2, height/2);
  rotateX(pitch);
  rotateZ(roll);
  
  box(150);
}

void serialEvent(Serial arduino){
  String[] lines = split(arduino.readStringUntil('\n'),'|');
  try{
    roll  = -float(lines[0]) * PI/180;
    pitch = -float(lines[1]) * PI/180;
  }catch(Exception e){
    roll = 0;
    pitch = 0;
  }
    
}

void mouseDragged(){
  //rotY -= (mouseX - pmouseX) * 0.01;
  //rotX -= (mouseY - pmouseY) * 0.01;
}
