#include <xpcc/architecture/platform.hpp>
#include <xpcc/debug/logger.hpp>

#include "thread_bme280.hpp"

// ----------------------------------------------------------------------------
// Set the log level
#undef	XPCC_LOG_LEVEL
#define	XPCC_LOG_LEVEL xpcc::log::INFO

// ----------------------------------------------------------------------------
Bme280Thread::Bme280Thread() :
	barometerA(dataA, 0x76),
	barometerB(dataB, 0x76),
	start_measurement(false),
	new_data(false)
{
}

bool
Bme280Thread::startMeasurement()
{
	start_measurement = true;
	new_data = false;
	return true;
}

bool
Bme280Thread::update()
{
	PT_BEGIN()

	XPCC_LOG_DEBUG << XPCC_FILE_INFO;
	XPCC_LOG_DEBUG << "Ping the BME280" << xpcc::endl;

	// ping the device until it responds
	while(true)
	{
		// we wait until the task started
		if (PT_CALL(barometerA.ping()))
			break;
		// otherwise, try again in 100ms
		this->timeout.restart(100);
		PT_WAIT_UNTIL(this->timeout.isExpired());
	}

	while(true)
	{
		// we wait until the task started
		if (PT_CALL(barometerB.ping()))
			break;
		// otherwise, try again in 100ms
		this->timeout.restart(100);
		PT_WAIT_UNTIL(this->timeout.isExpired());
	}

	// stream << "Barometer BME280 responded" << xpcc::endl;

	// Configure the device until it responds
	while(true)
	{
		// we wait until the task started
		if (PT_CALL(barometerA.initialize()))
			break;
		// otherwise, try again in 100ms
		this->timeout.restart(100);
		PT_WAIT_UNTIL(this->timeout.isExpired());
	}

	while(true)
	{
		// we wait until the task started
		if (PT_CALL(barometerB.initialize()))
			break;
		// otherwise, try again in 100ms
		this->timeout.restart(100);
		PT_WAIT_UNTIL(this->timeout.isExpired());
	}

	XPCC_LOG_DEBUG << XPCC_FILE_INFO;
	XPCC_LOG_DEBUG << "BME280 configured" << xpcc::endl;

	{
		static xpcc::bme280::Calibration &cal = dataA.getCalibration();

		XPCC_LOG_DEBUG << XPCC_FILE_INFO;
		XPCC_LOG_DEBUG << "BME280 A Calibration data is: " << xpcc::endl;
		XPCC_LOG_DEBUG.printf(" T1 %d\n", cal.T1);
		XPCC_LOG_DEBUG.printf(" T2 %d\n", cal.T2);
		XPCC_LOG_DEBUG.printf(" T3 %d\n", cal.T3);
		XPCC_LOG_DEBUG.printf(" P1 %d\n", cal.P1);
		XPCC_LOG_DEBUG.printf(" P2 %d\n", cal.P2);
		XPCC_LOG_DEBUG.printf(" P3 %d\n", cal.P3);
		XPCC_LOG_DEBUG.printf(" P4 %d\n", cal.P4);
		XPCC_LOG_DEBUG.printf(" P5 %d\n", cal.P5);
		XPCC_LOG_DEBUG.printf(" P6 %d\n", cal.P6);
		XPCC_LOG_DEBUG.printf(" P7 %d\n", cal.P7);
		XPCC_LOG_DEBUG.printf(" P8 %d\n", cal.P8);
		XPCC_LOG_DEBUG.printf(" P9 %d\n", cal.P9);
		XPCC_LOG_DEBUG.printf(" H1 %d\n", cal.H1);
		XPCC_LOG_DEBUG.printf(" H2 %d\n", cal.H2);
		XPCC_LOG_DEBUG.printf(" H3 %d\n", cal.H3);
		XPCC_LOG_DEBUG.printf(" H4 %d\n", cal.H4);
		XPCC_LOG_DEBUG.printf(" H5 %d\n", cal.H5);
		XPCC_LOG_DEBUG.printf(" H6 %d\n", cal.H6);
	}

	{
		static xpcc::bme280::Calibration &cal = dataB.getCalibration();

		XPCC_LOG_DEBUG << XPCC_FILE_INFO;
		XPCC_LOG_DEBUG << "BME280 B Calibration data is: " << xpcc::endl;
		XPCC_LOG_DEBUG.printf(" T1 %d\n", cal.T1);
		XPCC_LOG_DEBUG.printf(" T2 %d\n", cal.T2);
		XPCC_LOG_DEBUG.printf(" T3 %d\n", cal.T3);
		XPCC_LOG_DEBUG.printf(" P1 %d\n", cal.P1);
		XPCC_LOG_DEBUG.printf(" P2 %d\n", cal.P2);
		XPCC_LOG_DEBUG.printf(" P3 %d\n", cal.P3);
		XPCC_LOG_DEBUG.printf(" P4 %d\n", cal.P4);
		XPCC_LOG_DEBUG.printf(" P5 %d\n", cal.P5);
		XPCC_LOG_DEBUG.printf(" P6 %d\n", cal.P6);
		XPCC_LOG_DEBUG.printf(" P7 %d\n", cal.P7);
		XPCC_LOG_DEBUG.printf(" P8 %d\n", cal.P8);
		XPCC_LOG_DEBUG.printf(" P9 %d\n", cal.P9);
		XPCC_LOG_DEBUG.printf(" H1 %d\n", cal.H1);
		XPCC_LOG_DEBUG.printf(" H2 %d\n", cal.H2);
		XPCC_LOG_DEBUG.printf(" H3 %d\n", cal.H3);
		XPCC_LOG_DEBUG.printf(" H4 %d\n", cal.H4);
		XPCC_LOG_DEBUG.printf(" H5 %d\n", cal.H5);
		XPCC_LOG_DEBUG.printf(" H6 %d\n", cal.H6);
	}

	while (true)
	{
		PT_WAIT_UNTIL(start_measurement);

		// Returns when new data was read from the sensor
		PT_CALL(barometerA.readout());
		PT_CALL(barometerB.readout());
		new_data = true;

		{
			int32_t temp = dataA.getTemperature();
			int32_t press = dataA.getPressure();
			int32_t hum = dataA.getHumidity();
			XPCC_LOG_DEBUG << XPCC_FILE_INFO;
			XPCC_LOG_DEBUG.printf("BME280: Calibrated temperature in 0.01 degree Celsius is: %d\n", temp  );
			XPCC_LOG_DEBUG << XPCC_FILE_INFO;
			XPCC_LOG_DEBUG.printf("BME280: Calibrated pressure in mPa is                   : %d\n", press );
			XPCC_LOG_DEBUG << XPCC_FILE_INFO;
			XPCC_LOG_DEBUG.printf("BME280: Calibrated humidity in 0.001 %% is               : %d\n", hum );
		}


		start_measurement = false;
	}

	PT_END();
}
