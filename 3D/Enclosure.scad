include <lib/nutsnbolts/cyl_head_bolt.scad>;
use <display.scad>;
use <pcb.scad>;

$fa = 0.4;
$fs = 0.4;

module cube_rounded(size, radius) {
    width = size[0];
    height = size[1];
    depth = size[2];
    for (x=[radius, width-radius]) {
        for (y=[radius, height-radius]) {
            translate([x, y, 0]) cylinder(h=depth, r=radius);
        }
    }
    translate([radius, 0, 0]) cube([width-radius*2, height, depth]);
    translate([0, radius, 0]) cube([width, height-radius*2, depth]);
}

radius = 5;
width = 130;
height = 55;
depth = height - radius;

wall = 2;

screw_diam = 4;
screw_len = 20;
leg_size = screw_diam*3;
//screw("M4x20");

module chassis_screwholes() {
    for (x=[leg_size/2, width-leg_size/2]) {
        for (y=[leg_size/2, height-leg_size/2]) {
            translate([x, y, depth/2+screw_len-0.1]) hole_through("M4", l=20);
        }
    }
}

module chassis_screwlegs(clearance=0) {
    r = 3;
    for (x=[-clearance, width-leg_size+clearance]) {
        for (y=[-clearance, height-leg_size+clearance]) {
            translate([x, y, depth/2]) {
                cube_rounded([leg_size+2*clearance, leg_size+2*clearance, depth/2], radius);
            }
        }
    }
    for (x=[-clearance+leg_size-r, width+clearance-leg_size+r]) {
        for (y=[-clearance+leg_size-r, height+clearance-leg_size+r]) {
            translate([x, y, depth/2]) cylinder(h=depth/2, r=r);
        }
    }
}


// Front
module front() {
    difference() {
        union() {
            translate([0, 0, depth-wall]) cube_rounded([width, height, wall], radius);
            chassis_screwlegs();
        }
        chassis_screwholes();
    }
}

module back() {
    difference() {
        cube_rounded([width, height, depth], radius);
        translate([-0.01, -0.01, depth-wall+0.01]) cube_rounded([width+0.02, height+0.02, wall+0.02], radius);
        chassis_screwlegs(clearance=0.2);
        chassis_screwholes();
    }
}
front();
//back();

//display();
//translate([0, 0, -20]) pcb();
