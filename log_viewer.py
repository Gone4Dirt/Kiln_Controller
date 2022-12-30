import tkinter.filedialog as fd
import csv
import matplotlib.pyplot as plt

MILLIS_2_MIN = 1/60000

filename = fd.askopenfilename()

log_time_min = []
therm_profile_time_min = []
desired_temp = []
measured_time_min = []
board_temp = []
measure_temp = []

with open(filename, 'r') as file:
    lines = csv.reader(file)

    # count = 1
    have_header = False

    for data in lines:

        # Step over the header
        if not have_header:
            have_header = True
            continue
        # print(data)

        log_time_min.append(float(data[0])*MILLIS_2_MIN)

        therm_profile_time_min.append(float(data[1])*MILLIS_2_MIN)
        desired_temp.append(float(data[2]))

        measured_time_min.append(float(data[3])*MILLIS_2_MIN)
        board_temp.append(float(data[4]))
        measure_temp.append(float(data[5]))


        # count += 1

        # if count >= 50:
        #     break




# Plot the data
fig, ax1 = plt.subplots(figsize=(10, 5))

ax1.plot(therm_profile_time_min, desired_temp, label="Desired", linestyle='-', marker='', linewidth=1.5, color=[0,0,1])
ax1.plot(measured_time_min, board_temp, label="Board", linestyle='-', marker='', linewidth=1.5, color=[0.5,0.5,0.5])
ax1.plot(measured_time_min, measure_temp, label="Measured", linestyle='-', marker='', linewidth=1.5, color=[0,0,0])

ax1.set_xlabel('Time Elapsed (min)')
ax1.set_ylabel('Temp (deg C)')

ax1.legend(loc='upper right')



ax.plot(time_min, desired_temp, label="Des temp", linestyle='-', marker='', linewidth=1.5, color=[0,0,1])

ax.set_xlabel('Time Elapsed (min)')
ax.set_ylabel('Temp (deg C)')

ax.legend(loc='upper right')






plt.show()
