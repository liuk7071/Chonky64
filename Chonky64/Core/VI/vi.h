#include "../../Common/common.h"

class vi {
public:
	template<typename T> void write(u32 paddr, T data) {
		if constexpr (sizeof(T) == 4) {
			if (paddr == 0x04400000) ctrl = data;
			else if (paddr == 0x04400004) origin = data;
			else if (paddr == 0x04400008) width = data;
			else if (paddr == 0x0440000c) v_intr = data;
			else if (paddr == 0x04400010) return; // Writing anything to this register clears the currently triggered VI Interrupt
			else if (paddr == 0x04400014) burst = data;
			else if (paddr == 0x04400018) v_sync = data;
			else if (paddr == 0x0440001c) h_sync = data;
			else if (paddr == 0x04400020) h_sync_leap = data;
			else if (paddr == 0x04400024) h_video = data;
			else if (paddr == 0x04400028) v_video = data;
			else if (paddr == 0x0440002c) v_burst = data;
			else if (paddr == 0x04400030) x_scale = data;
			else if (paddr == 0x04400034) y_scale = data;
			else Helpers::panic("Unhandled VI 32bit write 0x%08x\n", paddr);
		}
		else if constexpr (sizeof(T) == 2) {
			Helpers::panic("Unhandled VI 16bit write 0x%08x\n", paddr);
		}
		else if constexpr (sizeof(T) == 1) {
			Helpers::panic("Unhandled VI 8bit write 0x%08x\n", paddr);
		}
	}
	template<typename T> T read(u32 paddr) {
		if constexpr (sizeof(T) == 8) {
			Helpers::panic("Unhandled VI 64bit read 0x%08x\n", paddr);
			return 0;
		}
		else if constexpr (sizeof(T) == 4) {
			Helpers::panic("Unhandled VI 32bit read 0x%08x\n", paddr);
			return 0;
		}
		else if constexpr (sizeof(T) == 2) {
			Helpers::panic("Unhandled VI 16bit read 0x%08x\n", paddr);
			return 0;
		}
		else if constexpr (sizeof(T) == 1) {
			Helpers::panic("Unhandled VI 8bit read 0x%08x\n", paddr);
			return 0;
		}
	}

	u32 ctrl = 0;
	u32 origin = 0;
	u32 width = 0;
	u32 v_intr = 0;
	u32 burst = 0;
	u32 v_sync = 0;
	u32 h_sync = 0;
	u32 h_sync_leap = 0;
	u32 h_video = 0;
	u32 v_video = 0;
	u32 v_burst = 0;
	u32 x_scale = 0;
	u32 y_scale = 0;
};