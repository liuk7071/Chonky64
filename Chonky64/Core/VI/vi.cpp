#include "vi.h"

void vi::step() {
	line_counter++;
	if (line_counter >= 6000) {
		current++;
		line_counter = 0;

		if (current == v_intr) {
			should_service_intr = true;
		}

		if (current >= 262) current = 0;
	}
}