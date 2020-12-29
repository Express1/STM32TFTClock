//lcd 2.8 box top
$fn = $preview ? 12 : 72;
w=75; //58+10
l=71; // 74+9.7 / 3/4
h=11+5;  //11

difference() {
polyhedron( points = [ [0,0,0], [l,0,0], [l,w,0], [0,w,0], [l*7/8,0,h], [l*7/8,w,h]], faces =[[0,1,2,3], [0,4,1], [0,3,5,4], [4,5,2,1], [3,2,5] ], convexity = 2);

#translate([20, 5, 0]) rotate(a=[0,-15,0]) cube([43.7,65,10],false);
}