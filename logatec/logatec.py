# This example shows how to send a spectrum sensing task to a node in the
# LOG-a-TEC testbed, wait for its completion and retrieve the results.

# You need vesna-alh-tools installed for this example to work. Follow the
# installation instructions at https://github.com/sensorlab/vesna-alh-tools.
from vesna import alh

# vesna-alh-tools uses the standard Python logging module to provide some debug
# information.
import logging

# A standard Python module, required for getting time-of-day and the delay
# function.
import time

# Import two classes from vesna-alh-tools we are going to use:
#
# A SpectrumSensor object represents a node in the LOG-a-TEC testbed that is
# equipped with a radio that can do spectrum sensing.
#
# A SpectrumSensorProgram object represents a spectrum sensing task.
from vesna.alh.spectrumsensor import SpectrumSensor, SpectrumSensorProgram

# The main function contains all the code for this experiment.
def main():

	# Turn on logging so that we can see requests and responses in the
	# terminal. This is useful to keep track of what is going on during the
	# experiment.
	logging.basicConfig(level=logging.INFO)

	# We must first create an object representing the coordinator node.
	# Each cluster in the LOG-a-TEC has its own coordinator and all
	# communication with the nodes must go through it.
	#
	# The parameters used here correspond to the LOG-a-TEC City Center
	# out-door cluster (see LOG-a-TEC documentation for other valid
	# options).
	#
	# Note that you must have a valid user name and password saved in an
	# "alhrc" file in order for this to work. See vesna-alh-tools README
	# for instructions on how to do that.
	#
	# Also make sure you reserved the cluster in the calendar before
	# running the experiment!
	coor = alh.ALHWeb("https://crn.log-a-tec.eu/communicator", 10002)

	# We will be using node 45 in this example. First we create a generic
	# node object for it.
	node = alh.ALHProxy(coor, 45)

	# We will use our node as a spectrum sensor. Here, we wrap the generic
	# node object with the SpectrumSensor class that provides a convenient
	# interface to the spectrum sensing capabilities of the node's radio
	# hardware.
	sensor = SpectrumSensor(node)

	# Request the list of spectrum sensing configurations that the node
	# supports.
	#
	# Node 45 is equipped with a SNE-ISMTV-2400 radio board that contains a
	# CC2500 reconfigurable transceiver. This tranceiver supports a number
	# of configurations that cover the 2.4 GHz ISM band.
	sensor_config_list = sensor.get_config_list()

	# Let's print the list out in a nice format for future reference.
	print
	print "Spectrum sensing devices and configurations available"
	print "====================================================="
	print sensor_config_list
	print


	# For a demonstration, we'll choose to sense the whole ISM band between
	# 2.4 GHz and 2.5 GHz with 400 kHz steps. We create a SweepConfig object
	# that describes this frequency range.
	sweep_config = sensor_config_list.get_sweep_config(2400e6, 2500e6, 400e3)
	if sweep_config is None:
		raise Exception("Node can not scan specified frequency range.")

	# Take note of current time.
	now = time.time()

	# SpectrumSensorProgram object allows us to program a node to perform a
	# spectrum sensing task in advance. We'll setup a task for sensing the
	# frequency range we selected above, starting 5 seconds from now and
	# lasting for 30 seconds.
	#
	# Each node has local slots for temporary storage of spectrum sensing
	# results. Since we are only performing one scan of the spectrum, it
	# doesn't matter which slot we use. Let's pick slot 1.
	sensor_program = SpectrumSensorProgram(sweep_config, now + 5, 30, 1)

	# Now we actually send the programming instructions over the testbed's
	# management network to the node.
	sensor.program(sensor_program)

	# Wait until the programmed task has been completed.
	while not sensor.is_complete(sensor_program):
		print "patience..."
		time.sleep(2)

	# The task is complete. We must now retrieve spectrum sensing results
	# back from the node to our computer. This might take a while since the
	# management mesh network is slow.
	result = sensor.retrieve(sensor_program)

	# Finally, we write results into a CSV file for further analysis.
	#
	# Alternatively, we could do something else with the results directly
	# from this program. The SpectrumSensorResult object provides some
	# convenient methods for accessing the result of the scan.
	result.write("logatec.csv")

	# The CSV file can be easily imported and plotted into tools like
	# MatLab. For example, to plot the recorded spectrogram using GNU Plot,
	# use the following commands:
	#
	# gnuplot> set pm3d map
	# gnuplot> set xlabel "frequency [MHz]"
	# gnuplot> set ylabel "time [s]"
	# gnuplot> set cblabel "power [dBm]"
	# gnuplot> unset key
	# gnuplot> splot "logatec.csv" using ($2/1e6):1:3

main()
