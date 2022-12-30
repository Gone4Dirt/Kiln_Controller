import tkinter.filedialog as fd
import csv
import matplotlib.pyplot as plt

MILLIS_2_MIN = 1/60000
MIN_2_SEC = 60

#--------------------------------------------------------------
# Calculate the delta time, input min, return sec
def calc_dt(time_min):
    dt = []
    for i in range(len(time_min)-1):
        dt.append((time_min[i+1] - time_min[i])*MIN_2_SEC)
    return dt
#--------------------------------------------------------------



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



# Calculate and plot the delta time on the various parts of the controller
logging_dt_s = calc_dt(log_time_min)
profile_dt_s = calc_dt(therm_profile_time_min)
measurement_dt_s = calc_dt(measured_time_min)


fig, ax2 = plt.subplots(figsize=(10, 5))

ax2.plot(log_time_min[1:], logging_dt_s, label="Logging", linestyle='-', marker='', linewidth=1.5, color=[0,0,1])
ax2.plot(therm_profile_time_min[1:], profile_dt_s, label="Therm Prof", linestyle='-', marker='', linewidth=1.5, color=[1,0,0])
ax2.plot(measured_time_min[1:], measurement_dt_s, label="Measurment", linestyle='-', marker='', linewidth=1.5, color=[0,1,0])

ax2.set_xlabel('Time Elapsed (min)')
ax2.set_ylabel('delta time (s)')

ax2.legend(loc='upper right')





plt.show()
