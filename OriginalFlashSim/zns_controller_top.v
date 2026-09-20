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

wire [31:0] znscontroller0;
wire [31:0] znscontroller1;
wire znscontroller2;
reg [5:0] znscontroller3 = 6'd6;
reg [31:0] znscontroller4 = 32'd63;
reg [31:0] znscontroller5 = 32'd4096;
wire [31:0] znscontroller6;
wire [31:0] znscontroller7;
wire [31:0] znscontroller8;
wire znscontroller9;


zns_metadata_bram zns_metadata_bram(
	.addr(znscontroller6[11:0]),
	.clk(sys_clk),
	.wdata(znscontroller8),
	.we(znscontroller9),
	.rdata(znscontroller7)
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
	.bram_rdata(znscontroller7),
	.clk(sys_clk),
	.io_cmd(csrstorage_2),
	.io_trigger(csrstorage_3),
	.rst(sys_rst),
	.validated_target_page(znscontroller1),
	.validated_zone_id(znscontroller0),
	.bram_addr(znscontroller6),
	.bram_wdata(znscontroller8),
	.bram_we(znscontroller9),
	.out_err_code(csrstatus_1),
	.out_status(csrstatus)
);

endmodule

