import tkinter.filedialog as fd
import csv
import matplotlib.pyplot as plt

MILLIS_2_MIN = 1/60000

filename = fd.askopenfilename()

time_min = []
desired_temp = []

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

        time_min.append(float(data[0])*MILLIS_2_MIN)
        desired_temp.append(float(data[1]))

        # count += 1

        # if count >= 50:
        #     break




# Plot the data
fig, ax = plt.subplots(figsize=(10, 5))

ax.plot(time_min, desired_temp, label="Des temp", linestyle='-', marker='', linewidth=1.5, color=[0,0,1])

ax.set_xlabel('Time Elapsed (min)')
ax.set_ylabel('Temp (deg C)')

ax.legend(loc='upper right')






plt.show()
