#include <iostream>
#include <fstream>
#include <cstdint>
#include <cstring>
#include <vector>

#define FRAME_SIZE 35
#define FRAME_DATA_SIZE 28

struct Frame {
	uint16_t Counter;
	uint16_t Length;
	uint8_t Data[FRAME_SIZE];
};

struct Telemetry {
	std::string Callsign;
	std::string SatelliteName;
	uint16_t TotalResetCounter;
	uint16_t SwapResetCounter;
	uint8_t BatteryVoltage;
	int8_t MCUTemperature;
	int8_t PATemperature;
	uint8_t ProcessorCurrent;
	uint8_t MCUVoltage3V3;
	uint8_t MCUVoltage1V2;
	int16_t AngularRateXAxis;
	int16_t AngularRateYAxis;
	int16_t AngularRateZAxis;
	bool AntennaBurnwire;
};

Frame parseFrame(uint8_t *in) {
	Frame frame;
	frame.Counter  = in[1] << 8 | in[2];
	frame.Length   = in[5] << 8 | in[6];
	std::memcpy(frame.Data, in, FRAME_SIZE);

	return frame;
}

void parseTelemetry(Frame in, int time) {
	Telemetry telm;

	telm.Callsign.append((char *)&in.Data[6], (char *)&in.Data[12]);
	telm.SatelliteName.append((char *)&in.Data[12], (char *)&in.Data[18]);
	telm.TotalResetCounter = in.Data[18] << 8 | in.Data[19];
	telm.SwapResetCounter = in.Data[20] << 8 | in.Data[21];
	telm.BatteryVoltage = in.Data[22];
	telm.MCUTemperature = in.Data[23];
	telm.PATemperature = in.Data[24];
	telm.ProcessorCurrent = in.Data[25];
	telm.MCUVoltage3V3 = in.Data[26];
	telm.MCUVoltage1V2 = in.Data[27];
	telm.AngularRateXAxis = in.Data[28] << 8 | in.Data[29];
	telm.AngularRateYAxis = in.Data[30] << 8 | in.Data[31];
	telm.AngularRateZAxis = in.Data[32] << 8 | in.Data[33];
	telm.AntennaBurnwire = in.Data[34] == 1;


	std::cout << "\"" << time << "\":{";

	std::cout << "\"callsign\":{";
	std::cout << "\"value\":\"" << telm.Callsign << "\"},";

	std::cout << "\"name\":{";
	std::cout << "\"value\":\"" << telm.SatelliteName << "\"},";

	std::cout << "\"reset_counter\":{";
	std::cout << "\"value\":" << (int)telm.TotalResetCounter << "},";

	std::cout << "\"swap_reset_counter\":{";
	std::cout << "\"value\":" << (int)telm.SwapResetCounter << "},";

	std::cout << "\"battery\":{";
	std::cout << "\"value\":" << telm.BatteryVoltage * 0.02f << ",\"unit\":\"V\"},";

	std::cout << "\"mcu_temp\":{";
	std::cout << "\"value\":" << (int)telm.MCUTemperature << ",\"unit\": \"°C\"},";

	std::cout << "\"pa_temp\":{";
	std::cout << "\"value\":" << (int)telm.PATemperature << ",\"unit\": \"°C\"},";

	std::cout << "\"mcu_current\":{";
	std::cout << "\"value\":" << (int)telm.ProcessorCurrent << ",\"unit\": \"mA\"},";

	std::cout << "\"mcu_3v3\":{";
	std::cout << "\"value\":" << telm.MCUVoltage3V3 * 0.02f << ",\"unit\": \"V\"},";

	std::cout << "\"mcu_1v2\":{";
	std::cout << "\"value\":" << telm.MCUVoltage1V2 * 0.02f << ",\"unit\": \"V\"},";

	std::cout << "\"angular_rate_x\":{";
	std::cout << "\"value\":" << (telm.AngularRateXAxis == 2001 ? "false" : std::to_string(telm.AngularRateXAxis)) << "},";

	std::cout << "\"angular_rate_y\":{";
	std::cout << "\"value\":" << (telm.AngularRateYAxis == 2001 ? "false" : std::to_string(telm.AngularRateYAxis)) << "},";

	std::cout << "\"angular_rate_z\":{";
	std::cout << "\"value\":" << (telm.AngularRateZAxis == 2001 ? "false" : std::to_string(telm.AngularRateZAxis)) << "},";
	
	std::cout <<  "\"antenna_burnwire\":{";
	std::cout << "\"value\":" << (telm.AntennaBurnwire ? "true" : "false") << "}";

	std::cout << "}";
}

int main(int argc, char *argv[]) {
	if(argc != 3) {
		std::cout << "Usage: " << argv[0] << " input.bin startTimeStamp" << std::endl;
		return 1;
	}

	std::ifstream data_in(argv[1], std::ios::binary);
	if(!data_in){
		std::cout << "Could not open input file" << std::endl;
		return 1;
	}

	int time = std::stoi(argv[2]);
	bool first = true;

	std::cout << "{\"satellite\":\"lucky7\",\"telemetry\":{";

	while (!data_in.eof()) {
		uint8_t buffer[FRAME_SIZE];
		data_in.read((char *)buffer, FRAME_SIZE);

		Frame frame = parseFrame(buffer);

		if (frame.Counter == 0){
			if (!first) {
				std::cout << ",";
			}

			parseTelemetry(frame, time);

			first = false;
		}

		time += 20;
	}

	std::cout << "}}";

	data_in.close();

	return 0;
}
