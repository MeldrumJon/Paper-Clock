include <lib/nutsnbolts/cyl_head_bolt.scad>;
use <display.scad>;
use <pcb.scad>;

$fa = 0.4;
$fs = 0.4;

wall = 2;

roundness = 5;
width = 130;
height = 55;
depth = height - roundness;

// M4x20 screws for enclosure
screw_diam = 4;
screw_len = 20;
leg_size = screw_diam*3;
//screw("M4x20");

// Front
module front() {
    translate([0, 0, depth-wall]) cube_rounded([width, height, wall], roundness);
    translate([0, 0, depth/2]) 
        rotate([0, 0, 0])  screw_leg(leg_size, depth/2, roundness, 3);
    translate([width, 0, depth/2]) 
        rotate([0, 0, 90])  screw_leg(leg_size, depth/2, roundness, 3);
    translate([width, height, depth/2])
        rotate([0, 0, 180]) screw_leg(leg_size, depth/2, roundness, 3);
    translate([0, height, depth/2])
        rotate([0, 0, 270]) screw_leg(leg_size, depth/2, roundness, 3);
}
difference() {
    front();
    for (x=[leg_size/2, width-leg_size/2]) {
        for (y=[leg_size/2, height-leg_size/2]) {
            translate([x, y, depth/2+screw_len-0.2]) hole_through("M4", l=20);
        }
    }
}

module cube_rounded(size, roundness, hollow=false, thickness) {
    width = size[0];
    height = size[1];
    depth = size[2];

    union() {
        for (x=[roundness, width-roundness]) {
            for (y=[roundness, height-roundness]) {
                translate([x, y, 0]) cylinder(h=depth, r=roundness);
            }
        }
        translate([roundness, 0, 0]) cube([width-roundness*2, height, depth]);
        translate([0, roundness, 0]) cube([width, height-roundness*2, depth]);
    }
}

module screw_leg(w, h, r1, r2) {
    translate([r1, r1, 0]) cylinder(h=h, r=r1);
    translate([w-r2,w-r2,0]) cylinder(h=h, r=r2);
    translate([roundness, 0, 0]) cube([w-r1, w-r2, h]);
    translate([0, roundness, 0]) cube([w-r2, w-r1, h]);
}

//display();
//translate([0, 0, -20]) pcb();
