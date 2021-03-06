defApplication("ath_spec_scan","ath_spec_scan"){ |app|
	app.version(1, 0, 0)
	app.shortDescription = 'Atheros spectral scanning program'
	app.path = "ath_spec_scan"

	app.defProperty('phy_name', 'physical interface name of the wireless card (default: phy0)', '-p', :type => :string, :mnemonic => 'p')
	app.defProperty('fft_period', 'when active and triggered, PHY passes FFT frames to MAC every (fft_period+1)*4uS (default: 15)', '-f', {:type => :integer, :mnemonic => 'f'})
	app.defProperty('period', 'time period between successive spectral scan entry points (period*256*Tclk) (default: 1)', '-P', {:type => :integer, :mnemonic => 'P'})
	app.defProperty('hold_mode', 'data holding mode (i.e. 0 => MAX_HOLD, 1 => MIN_HOLD and 2 => AVG_HOLD) (default: 0)', '-m', {:type => :integer, :mnemonic => 'm'})
	app.defProperty('noise_thld', 'Wi-Fi noise threshold (dBm) for COR calculation (default: -95)', '-t', {:type => :double, :mnemonic => 't'})
	app.defProperty('intval', 'interval (msec) of a periodic report (default: 100)', '-i', {:type => :integer, :mnemonic => 'i'})

	app.defMeasurement("sample"){ |mp|
		mp.defMetric('sniffer_mac', 'string')
		mp.defMetric('freq', 'uint32')
		mp.defMetric('cor', 'double')
		mp.defMetric('wifi_energy', 'double')
		mp.defMetric('zigbee1_energy', 'double')
		mp.defMetric('zigbee2_energy', 'double')
		mp.defMetric('zigbee3_energy', 'double')
		mp.defMetric('zigbee4_energy', 'double')
		mp.defMetric('psd1', 'double')
		mp.defMetric('psd2', 'double')
		mp.defMetric('psd3', 'double')
		mp.defMetric('psd4', 'double')
		mp.defMetric('psd5', 'double')
		mp.defMetric('psd6', 'double')
		mp.defMetric('psd7', 'double')
		mp.defMetric('psd8', 'double')
		mp.defMetric('psd9', 'double')
		mp.defMetric('psd10', 'double')
		mp.defMetric('psd11', 'double')
		mp.defMetric('psd12', 'double')
		mp.defMetric('psd13', 'double')
		mp.defMetric('psd14', 'double')
		mp.defMetric('psd15', 'double')
		mp.defMetric('psd16', 'double')
		mp.defMetric('psd17', 'double')
		mp.defMetric('psd18', 'double')
		mp.defMetric('psd19', 'double')
		mp.defMetric('psd20', 'double')
		mp.defMetric('psd21', 'double')
		mp.defMetric('psd22', 'double')
		mp.defMetric('psd23', 'double')
		mp.defMetric('psd24', 'double')
		mp.defMetric('psd25', 'double')
		mp.defMetric('psd26', 'double')
		mp.defMetric('psd27', 'double')
		mp.defMetric('psd28', 'double')
		mp.defMetric('psd29', 'double')
		mp.defMetric('psd30', 'double')
		mp.defMetric('psd31', 'double')
		mp.defMetric('psd32', 'double')
		mp.defMetric('psd33', 'double')
		mp.defMetric('psd34', 'double')
		mp.defMetric('psd35', 'double')
		mp.defMetric('psd36', 'double')
		mp.defMetric('psd37', 'double')
		mp.defMetric('psd38', 'double')
		mp.defMetric('psd39', 'double')
		mp.defMetric('psd40', 'double')
		mp.defMetric('psd41', 'double')
		mp.defMetric('psd42', 'double')
		mp.defMetric('psd43', 'double')
		mp.defMetric('psd44', 'double')
		mp.defMetric('psd45', 'double')
		mp.defMetric('psd46', 'double')
		mp.defMetric('psd47', 'double')
		mp.defMetric('psd48', 'double')
		mp.defMetric('psd49', 'double')
		mp.defMetric('psd50', 'double')
		mp.defMetric('psd51', 'double')
		mp.defMetric('psd52', 'double')
		mp.defMetric('psd53', 'double')
		mp.defMetric('psd54', 'double')
		mp.defMetric('psd55', 'double')
		mp.defMetric('psd56', 'double')
		mp.defMetric('duration', 'uint32')
	}
}
