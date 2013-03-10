
#ifndef GAINPUTPADINFO_H_
#define GAINPUTPADINFO_H_

namespace gainput
{

namespace
{
struct DeviceButtonInfo
{
	ButtonType type;
	const char* name;
};

DeviceButtonInfo deviceButtonInfos[] =
{
		{ BT_FLOAT, "pad_left_stick_x" },
		{ BT_FLOAT, "pad_left_stick_y" },
		{ BT_FLOAT, "pad_right_stick_x" },
		{ BT_FLOAT, "pad_right_stick_y" },
		{ BT_FLOAT, "pad_axis_4" },
		{ BT_FLOAT, "pad_axis_5" },
		{ BT_FLOAT, "pad_axis_6" },
		{ BT_FLOAT, "pad_axis_7" },
		{ BT_FLOAT, "pad_axis_8" },
		{ BT_FLOAT, "pad_axis_9" },
		{ BT_FLOAT, "pad_axis_10" },
		{ BT_FLOAT, "pad_axis_11" },
		{ BT_FLOAT, "pad_axis_12" },
		{ BT_FLOAT, "pad_axis_13" },
		{ BT_FLOAT, "pad_axis_14" },
		{ BT_FLOAT, "pad_axis_15" },
		{ BT_FLOAT, "pad_axis_16" },
		{ BT_FLOAT, "pad_axis_17" },
		{ BT_FLOAT, "pad_axis_18" },
		{ BT_FLOAT, "pad_axis_19" },
		{ BT_FLOAT, "pad_axis_20" },
		{ BT_FLOAT, "pad_axis_21" },
		{ BT_FLOAT, "pad_axis_22" },
		{ BT_FLOAT, "pad_axis_23" },
		{ BT_FLOAT, "pad_axis_24" },
		{ BT_FLOAT, "pad_axis_25" },
		{ BT_FLOAT, "pad_axis_26" },
		{ BT_FLOAT, "pad_axis_27" },
		{ BT_FLOAT, "pad_axis_28" },
		{ BT_FLOAT, "pad_axis_29" },
		{ BT_FLOAT, "pad_axis_30" },
		{ BT_FLOAT, "pad_axis_31" },
		{ BT_FLOAT, "pad_acceleration_x" },
		{ BT_FLOAT, "pad_acceleration_y" },
		{ BT_FLOAT, "pad_acceleration_z" },
		{ BT_BOOL, "pad_button_start"},
		{ BT_BOOL, "pad_button_select"},
		{ BT_BOOL, "pad_button_left"},
		{ BT_BOOL, "pad_button_right"},
		{ BT_BOOL, "pad_button_up"},
		{ BT_BOOL, "pad_button_down"},
		{ BT_BOOL, "pad_button_a"},
		{ BT_BOOL, "pad_button_b"},
		{ BT_BOOL, "pad_button_x"},
		{ BT_BOOL, "pad_button_y"},
		{ BT_BOOL, "pad_button_l1"},
		{ BT_BOOL, "pad_button_r1"},
		{ BT_BOOL, "pad_button_l2"},
		{ BT_BOOL, "pad_button_r2"},
		{ BT_BOOL, "pad_button_l3"},
		{ BT_BOOL, "pad_button_r3"},
		{ BT_BOOL, "pad_button_home"},
		{ BT_BOOL, "pad_button_17"},
		{ BT_BOOL, "pad_button_18"},
		{ BT_BOOL, "pad_button_19"},
		{ BT_BOOL, "pad_button_20"},
		{ BT_BOOL, "pad_button_21"},
		{ BT_BOOL, "pad_button_22"},
		{ BT_BOOL, "pad_button_23"},
		{ BT_BOOL, "pad_button_24"},
		{ BT_BOOL, "pad_button_25"},
		{ BT_BOOL, "pad_button_26"},
		{ BT_BOOL, "pad_button_27"},
		{ BT_BOOL, "pad_button_28"},
		{ BT_BOOL, "pad_button_29"},
		{ BT_BOOL, "pad_button_30"},
		{ BT_BOOL, "pad_button_31"}
};
}

const unsigned PadButtonCount = PAD_BUTTON_COUNT;
const unsigned PadAxisCount = PAD_BUTTON_AXIS_COUNT;

}

#endif

