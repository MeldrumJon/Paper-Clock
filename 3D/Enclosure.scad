include <lib/nutsnbolts/cyl_head_bolt.scad>;
use <display.scad>;
use <pcb.scad>;
use <mylib.scad>;

$fa = 0.4;
$fs = 0.4;

wall = 2;
guide_width = wall/2;
guide_height = 2;
guide_clearance = 0.4;

roundness = 5;
width = 130;
height = 55;
depth = height - roundness;

// M4x20 screws for enclosure
screw_diam = 4;
screw_len = 20;
leg_size = screw_diam*3+screw_diam/2;
screw_place = screw_diam/2+screw_diam*3/2;
//screw("M4x20");

module screw_leg(w, h, r1, r2) {
    translate([r1, r1, 0]) cylinder(h=h, r=r1);
    translate([w-r2,w-r2,0]) cylinder(h=h, r=r2);
    translate([roundness, 0, 0]) cube([w-r1, w-r2, h]);
    translate([0, roundness, 0]) cube([w-r2, w-r1, h]);
}

// Front
module front() {
    translate([0, 0, depth-wall])
        rounded_cube([width, height, wall], roundness);
    translate([0, 0, depth/2])
        rounded_cube_pipe([width, height, depth/2], roundness=roundness, wall_thickness=wall);

    guide_width = guide_width - guide_clearance;
    padding = wall - guide_width;
    translate([padding, padding, depth/2-guide_height])
        rounded_cube_pipe([width-padding*2, height-padding*2, depth/2+guide_height], roundness=roundness, wall_thickness=guide_width);

    translate([0, 0, depth/2]) 
        rotate([0, 0, 0])  screw_leg(leg_size, depth/2, roundness, 3);
    translate([width, 0, depth/2]) 
        rotate([0, 0, 90])  screw_leg(leg_size, depth/2, roundness, 3);
    translate([width, height, depth/2])
        rotate([0, 0, 180]) screw_leg(leg_size, depth/2, roundness, 3);
    translate([0, height, depth/2])
        rotate([0, 0, 270]) screw_leg(leg_size, depth/2, roundness, 3);
}

module back() {
    rounded_cube_pipe([width, height, depth/2-guide_height], roundness=roundness, wall_thickness=wall);
    rounded_cube_pipe([width, height, depth/2], roundness=roundness, wall_thickness=guide_width);
}

difference() {
    union() {
        translate([0, 0, 5]) color("orange") front();
        back();
    }
    for (x=[screw_place, width-screw_place]) {
        for (y=[screw_place, height-screw_place]) {
            translate([x, y, depth/2+screw_len-0.2]) hole_through("M4", l=20);
        }
    }
}

//display();
//translate([0, 0, -20]) pcb();
