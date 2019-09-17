include <lib/nutsnbolts/cyl_head_bolt.scad>;
include <display.scad>;
use <pcb.scad>;
use <mylib.scad>;

$fa = 0.4;
$fs = 0.4;

// Case

wall = 2;

outer_roundness = 5;
outer_width = 130;
outer_height = 55;
outer_depth = outer_height - outer_roundness;

clearance = 0.6;

inner_offset = (wall+clearance);

inner_width = outer_width - 2*inner_offset;
inner_height = outer_height - 2*inner_offset;
inner_depth = outer_depth - inner_offset;
inner_roundness = outer_roundness - inner_offset;

// M4x20 screws for enclosure: screw("M4x20");
screw_diam = 4;
screw_len = 20;

// Front
module front() {
    // Face
    difference() {
        translate ([0, 0, outer_depth-wall])
            rounded_cube([outer_width, outer_height, wall], roundness=outer_roundness);
        translate ([outer_width/2 - display_screen_width/2, outer_height/2 - display_screen_height/2, outer_depth-wall-0.1])
            cube([display_screen_width, display_screen_height, wall+0.2]);
    }
    // Walls
    rounded_cube_pipe([outer_width, outer_height, outer_depth], wall_thickness=wall, roundness=outer_roundness);

    // Screw columns
    screw_width = screw_diam*3;
    screw_bot_clearance = wall+0.1;
    screw_height = outer_depth-screw_bot_clearance;
    screw_r1 = 0.6;
    screw_r2 = 1.8;
    screw_offset = inner_offset + wall+clearance;
    translate([screw_offset, screw_offset, screw_bot_clearance]) 
        rotate([0, 0, 0])  screw_leg(screw_width, screw_height, screw_r1, screw_r2);
    translate([outer_width-screw_offset, screw_offset, screw_bot_clearance]) 
        rotate([0, 0, 90])  screw_leg(screw_width, screw_height, screw_r1, screw_r2);
    translate([outer_width-screw_offset, outer_height-screw_offset, screw_bot_clearance])
        rotate([0, 0, 180]) screw_leg(screw_width, screw_height, screw_r1, screw_r2);
    translate([screw_offset, outer_height-screw_offset, screw_bot_clearance])
        rotate([0, 0, 270]) screw_leg(screw_width, screw_height, screw_r1, screw_r2);
}

module back() {
    translate ([inner_offset, inner_offset, 0]) {
        // Face
        rounded_cube([inner_width, inner_height, wall], roundness=inner_roundness);
        // Walls
        rounded_cube_pipe([inner_width, inner_height, inner_depth], wall_thickness=wall, roundness=inner_roundness);
    }
}

//translate([0, 0, outer_depth*2])
    front();
back();
color("orange") case_screws();

//display();
//translate([0, 0, -20]) pcb();

module case_screws() {
    offset = inner_offset + wall+clearance + screw_diam*3/2;

    for (x=[offset, outer_width-offset], y=[offset, outer_height-offset]) {
        translate([x, y, -0.1])
            rotate([180,0,0]) hole_threaded("M4", 20, thread="no");
    }
}

module screw_leg(w, h, r1, r2) {
    translate([r1, r1, 0]) cylinder(h=h, r=r1);
    translate([w-r2,w-r2,0]) cylinder(h=h, r=r2);
    translate([r1, 0, 0]) cube([w-r1, w-r2, h]);
    translate([0, r1, 0]) cube([w-r2, w-r1, h]);
}
