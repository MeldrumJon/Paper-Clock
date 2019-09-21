include <lib/BOSL/constants.scad>
use <lib/BOSL/shapes.scad>
include <lib/nutsnbolts/cyl_head_bolt.scad>;
include <display.scad>;
include <pcb.scad>;
include <encoder.scad>;
use <mylib.scad>;

$fa = 0.1;
$fs = 0.1;

// Case

wall = 2;

outer_roundness = 5;
outer_width = 140;
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

screen_overlap = 0.6;
display_x = (outer_width - display_screen_width)/2 - display_screen_start_x;
display_y = (outer_height - display_screen_height)/2 - display_screen_start_y;
display_z = outer_depth - wall - display_depth - clearance; 

display_column_inner_start_x = display_x+display_screw_dist+display_screw_diam*3/2;
display_column_inner_end_x = display_x+display_width-display_screw_dist-display_screw_diam*3/2;

pcb_x = display_column_inner_end_x-pcb_height-clearance;
pcb_y = (outer_height - pcb_width)/2;

pcb_lift = pcb_depth+pcb_usb_depth+clearance-wall;
pcb_lift_diam = 4;
pcb_lift_offset = 5;
pcb_lift_y = pcb_y + 19.5;

encoder_area_width = pcb_x - display_column_inner_start_x;
encoder_knob_length = 15;

encoder_wall = 2;
encoder_radius = 5;

encoder_outer_width = encoder_area_width - 2;
encoder_outer_height = encoder_area_width - 2;
encoder_outer_depth = encoder_knob_length+encoder_wall;

encoder_inner_width = encoder_outer_width - 2*encoder_wall;
encoder_inner_height = encoder_outer_height - 2*encoder_wall;
encoder_inner_depth = encoder_outer_depth - 2*encoder_wall;

encoder_x = display_column_inner_start_x + encoder_area_width/2;
encoder_y = outer_height/2;

// Front
module front() {
    // Face
    difference() {
        translate ([0, 0, outer_depth-wall])
            rounded_cube([outer_width, outer_height, wall], roundness=outer_roundness);

        border = wall;
        big_width = display_screen_width+2*border;
        big_height = display_screen_height+2*border;
        small_width = display_screen_width-2*screen_overlap;
        small_height = display_screen_height-2*screen_overlap;
        translate ([outer_width/2, outer_height/2, outer_depth-wall-0.1])
            prismoid([small_width, small_height], [big_width, big_height], wall+0.2);
    }
    // Walls
    *rounded_cube_pipe([outer_width, outer_height, outer_depth], wall_thickness=wall, roundness=outer_roundness);

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
    difference() {
        union() {
            translate ([inner_offset, inner_offset, 0]) {
                // Face
                rounded_cube([inner_width, inner_height, wall], roundness=inner_roundness);
                // Walls
                *rounded_cube_pipe([inner_width, inner_height, inner_depth], wall_thickness=wall, roundness=inner_roundness);
            }

            for (x=[display_x+display_screw_dist, display_x+display_width-display_screw_dist],
                 y=[display_y+display_screw_dist, display_y+display_height-display_screw_dist]) {
                translate([x, y, 0])
                    cylinder(d=3*display_screw_diam, h=display_z);
            }

            translate([encoder_x, encoder_y, encoder_outer_depth/2])
                rounded_cube([encoder_outer_width, encoder_outer_height, encoder_outer_depth],
                roundness=encoder_radius+encoder_wall, center=true);

            y = pcb_y + pcb_screw_offset;
            for (x=[pcb_x + pcb_screw_offset, pcb_x + pcb_height - pcb_screw_offset]) {
                translate([x, y, 0]) {
                    cylinder(d=pcb_screw_diam*3, h=pcb_usb_depth);
                }
            }

            translate([pcb_x+pcb_lift_offset, pcb_lift_y, 0])
                cylinder(d=pcb_lift_diam, h=pcb_usb_depth);
            translate([pcb_x+pcb_height-pcb_lift_offset, pcb_lift_y, 0])
                cylinder(d=pcb_lift_diam, h=pcb_usb_depth);
        }
        translate([encoder_x, encoder_y, encoder_inner_depth/2-0.1])
            rounded_cube([encoder_inner_width, encoder_inner_height, encoder_inner_depth+0.2], roundness=encoder_radius, center=true);
        translate([encoder_x, encoder_y, 0])
            cylinder(d=encoder_pole_diam, h=encoder_outer_depth+0.1);
        translate([pcb_x+pcb_height-pcb_usb_y, pcb_y+pcb_usb_x, 0])
            cube([pcb_usb_height+2, pcb_usb_width+2, outer_depth/2], center=true);
    }
}

module screws_case() {
    offset = inner_offset + wall+clearance + screw_diam*3/2;

    for (x=[offset, outer_width-offset], y=[offset, outer_height-offset]) {
        translate([x, y, -0.1])
            rotate([180,0,0]) hole_threaded("M4", 20, thread="no");
    }
}

module screws_display() {
    for (x=[display_x+display_screw_dist, display_x+display_width-display_screw_dist],
         y=[display_y+display_screw_dist, display_y+display_height-display_screw_dist]) {
        translate([x, y, display_z+display_pcb_depth])
            hole_threaded(display_screw_name, 25, thread="no");
    }
}

module screws_pcb() {
    y = pcb_y + pcb_screw_offset;
    for (x=[pcb_x + pcb_screw_offset, pcb_x + pcb_height - pcb_screw_offset]) {
        translate([x, y, pcb_depth+pcb_usb_depth]) {
            hole_threaded(pcb_screw_name, 6, thread="no");
        }
    }
}

module screw_leg(w, h, r1, r2) {
    translate([r1, r1, 0]) cylinder(h=h, r=r1);
    translate([w-r2,w-r2,0]) cylinder(h=h, r=r2);
    translate([r1, 0, 0]) cube([w-r1, w-r2, h]);
    translate([0, r1, 0]) cube([w-r2, w-r1, h]);
}

// Case

//translate([0, 0, outer_depth*2])
    front();
back();
color("orange") {
    screws_case();
    screws_display();
    screws_pcb();
}

// Components
translate([display_x, display_y, display_z])
    display();
translate([pcb_x, pcb_y, pcb_depth+pcb_usb_depth+clearance-wall]) 
    translate([pcb_height, 0, 0]) 
        rotate([0, 0, 90])
        pcb();
translate([encoder_x, encoder_y, encoder_pcb_depth+encoder_box_depth+encoder_outer_depth])
    rotate([180, 0, 90]) encoder();


