
encoder_pcb_width = 26;
encoder_pcb_height = 16;
encoder_pcb_depth = 1.6;

encoder_pole_diam = 7;
encoder_pole_height = 28.6;

encoder_box_width = 12.5;
encoder_box_height = 12.5;
encoder_box_depth = 8.5 - encoder_pcb_depth;

module encoder() {
    translate([0, 0, encoder_pcb_depth])
        color("purple") import("res/rotaryEncoderPCB.stl");
    translate([-encoder_box_width/2, -encoder_box_height/2, encoder_pcb_depth])
        color("silver") cube([encoder_box_width, encoder_box_height, encoder_box_depth]);    
    translate([0, 0, 0.1])
        color("silver") cylinder(d=encoder_pole_diam, h=encoder_pole_height-0.1);
}

