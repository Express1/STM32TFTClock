// tftlcdClock back cover
$fn = $preview ? 12 : 72;

// inside size
wid1 = 60;
len1= 90+10;
height = 10;
wall = 2;
gap = 0.1;
cornerRadius = 10;

//buttons
 s=2.54;
 g=0.2;
 sy=11;  




//top

translate([0 , 0, 0]){
    mirror([0,0,0]) {
        //roundedBox(wid1, len1, 1, cornerRadius);
       difference() {
            roundedBox(wid1+2*wall, len1+2*wall, height/2+wall, cornerRadius);
            union() {
            translate([wall,wall,wall]) roundedBox(wid1, len1, height/2, cornerRadius);
            // holes 
            #for (a2 =[0:4]) for (a1 =[1:9]) translate([wall+5+a2*6,a1*6-1,0]) cube(size=[4,4, wall]);  //cylinder(wall,d=3)
            #for (a2 =[0:4]) for (a1 =[1:9]) translate([wall+70+a2*6,a1*6-1,0]) cube(size=[4,4, wall]); //cylinder(wall,d=3)
            // buzzer    
            *translate([50,29,0]) cylinder(wall,d=3);
            }
    }
    // 1 mm thick, 4 mm high
    // gap so we can close the box easy
    
    translate([wall-1,wall-1,height/2+wall]) {    
    difference() {
            roundedBox(wid1+2-gap,len1+2-gap,4,cornerRadius);
            translate([1,1,0]) {
                roundedBox(wid1,len1,4,cornerRadius);
            }    
        }
    }
  }

//buzzer

    *difference() {
        translate([50,29,wall]) cylinder(r=27/2+1,h=4,center=false);
        union() {
            translate([50,29,wall])cylinder(r=27/2-1,h=4,center=false);
            translate([50,29,wall+3])cylinder(r=27/2+0.2,h=1,center=false);
            }    
        }

}



module roundedBox(wid1, len1, height, radius)
{
    dRadius = 2*radius;
    cube(size=[len1,wid1, height]);
  
}


