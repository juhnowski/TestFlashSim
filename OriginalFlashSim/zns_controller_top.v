/* Machine-generated using Migen */
module top(
	input [31:0] csrstorage,
	input [31:0] csrstorage_1,
	input [7:0] csrstorage_2,
	input csrstorage_3,
	output [7:0] csrstatus,
	output [7:0] csrstatus_1,
	input sys_clk,
	input sys_rst
);

wire [7:0] znscontroller_csrstatus2;
wire [31:0] znscontroller0;
wire [31:0] znscontroller1;
wire znscontroller2;
reg [5:0] znscontroller3 = 6'd6;
reg [31:0] znscontroller4 = 32'd63;
reg [31:0] znscontroller5 = 32'd4096;
wire znscontroller6;
wire znscontroller7;
reg [7:0] znscontroller8 = 8'd35;
wire [31:0] znscontroller9;
wire [31:0] znscontroller10;
wire [31:0] znscontroller11;
wire znscontroller12;

// synthesis translate_off
reg dummy_s;
initial dummy_s <= 1'd0;
// synthesis translate_on

assign znscontroller_csrstatus2 = znscontroller8;

zns_thermal_manager zns_thermal_manager(
	.clk(sys_clk),
	.raw_temperature(znscontroller8),
	.rst(sys_rst),
	.out_thermal_err_code(znscontroller7),
	.out_thermal_shutdown_tripped(znscontroller6)
);

zns_metadata_bram zns_metadata_bram(
	.addr(znscontroller9[11:0]),
	.clk(sys_clk),
	.wdata(znscontroller11),
	.we(znscontroller12),
	.rdata(znscontroller10)
);

zns_address_resolver zns_address_resolver(
	.cfg_total_zones(znscontroller5),
	.cfg_zone_shift(znscontroller3),
	.cfg_zone_size_mask(znscontroller4),
	.clk(sys_clk),
	.io_lba({csrstorage_1, csrstorage}),
	.rst(sys_rst),
	.out_error_out_of_bounds(znscontroller2),
	.out_target_page(znscontroller1),
	.out_zone_id(znscontroller0)
);

zns_fsm_validator zns_fsm_validator(
	.addr_bound_error(znscontroller2),
	.bram_rdata(znscontroller10),
	.clk(sys_clk),
	.io_cmd(csrstorage_2),
	.io_trigger(csrstorage_3),
	.rst(sys_rst),
	.thermal_shutdown_tripped(znscontroller6),
	.validated_target_page(znscontroller1),
	.validated_zone_id(znscontroller0),
	.bram_addr(znscontroller9),
	.bram_wdata(znscontroller11),
	.bram_we(znscontroller12),
	.out_err_code(csrstatus_1),
	.out_status(csrstatus)
);

endmodule

