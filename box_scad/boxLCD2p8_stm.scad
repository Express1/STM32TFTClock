//lcd 2.8 box top
$fn = $preview ? 12 : 72;
//
// inside size
wid1 = 60; //54
len1= 90+10; // 100
height = 70; // 50 height/2
wall = 2;
gap = 0.05;
cornerRadius = 10;

//buttons
 s=2.54;
 g=0.2;
 sy=11;  


//bottom
difference() {
    translate([0, 0, 0]) roundedBox(wid1+2*wall, len1+2*wall, height/2+wall,cornerRadius);
    union() { 
        translate([wall,wall,wall]) roundedBox(wid1, len1, height/2, cornerRadius); 
        translate([wall-1,wall-1,height/2+wall-4-gap]) roundedBox(wid1+2,len1+2,4+gap,cornerRadius);
        //lcd hole
        translate([17+2+wall-0.5,(wid1-54)/2+4+wall,0]) cube(size=[61,46,wall]);
        
         //buzzer hole
         translate ([50,wall,wall+height/4-3]) rotate(a=[90,0,0]) cylinder(r=1.5,h=2,center=false);  
         // USB hole
         *translate ([18+2+wall,wall,7.3+wall]) rotate(a=[90,0,0]) cube(size=[10,5,wall]);      
        }
    } 



//LCD feet
 translate([9+wall,(wid1-54)/2+5+wall,wall]) {
    cylinder(6,d=2.5);
        }
translate([85+wall,(wid1-54)/2+5+wall,wall]) {
    cylinder(6,d=2.5);
        }
        translate([9+wall,(wid1-54)/2+49+wall,wall]) {
    cylinder(6,d=2.5);
        }
        translate([85+wall,(wid1-54)/2+49+wall,wall]) {
    cylinder(6,d=2.5);
        }

// gps rail
    gapr= 0.8; 
    translate([wall+len1-20,wall,wall]) cube(size=[2,2,height/2-4]);
    translate([wall+len1-20-gapr-2,wall,wall]) cube(size=[2,2,height/2-4]);
    
    translate([wall+len1-20,wid1,wall]) cube(size=[2,2,height/2-4]);
    translate([wall+len1-20-gapr-2,wid1,wall]) cube(size=[2,2,height/2-4]);  
     
    translate([wall+len1-35,wall,wall]) cube(size=[2,2,height/2-4]);
    translate([wall+len1-35-gapr-2,wall,wall]) cube(size=[2,2,height/2-4]);
    
    translate([wall+len1-35,wid1,wall]) cube(size=[2,2,height/2-4]);
    translate([wall+len1-35-gapr-2,wid1,wall]) cube(size=[2,2,height/2-4]);     
        



//buzzer
    translate ([50,4,wall+height/4-3])
    rotate(a=[90,0,0]) {
    difference() {
        cylinder(r=27/2+1,h=2,center=false);
        union() {
            cylinder(r=25/2,h=2,center=false);
            cylinder(r=27.2/2+0.1,h=0.5,center=false);
            } 
    }
}  




module roundedBox(wid1, len1, height, radius)
{
    dRadius = 2*radius;
    cube(size=[len1,wid1, height]);
  
}


