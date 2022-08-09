import os

for randomness in range(1):
    os.system("python generate_arrivals_new_500vph.py")
    print("\n\n\n\n AIMPBS")
    for step in range(100):
        commandPBS = "../PBS-SIPP-ver2/AIMPBS " + str(step)
        # os.system(commandPBS)
        time_running = os.popen(commandPBS).read()
        print(time_running[:-1])
        f = open("result/500vph/AIMPBSRunTime.txt", "a")
        f.write(time_running[:-1])
        f.write("\n")
        f.close()

        os.system("cp output.txt output_PBSSIPP.txt")

        with open("output.txt") as f:
            next(f)
            counter = 0

            max_speed = 15
            delay = 0
            throughput = 0
            in_time_min = 10000
            out_time_tail_max = 0
            for line_in_file in f:
                counter+=1
                data = line_in_file.split(";")

                in_point = data[0]
                out_point = data[1]
                length = float(data[2])
                earliest_arrival = float(data[3])
                in_time = float(data[4])
                out_time_head = float(data[5])
                out_time_tail = float(data[6])
                time_spend = float(data[7])
                speed = float(data[8])

                delay += out_time_head - earliest_arrival - length/max_speed

                if (in_time_min > in_time):
                    in_time_min = in_time
                if (out_time_tail_max < out_time_tail ):
                    out_time_tail_max = out_time_tail

                # throughput += out_time_tail - in_time
        delay = delay/counter
        # print(counter, out_time_tail_max, in_time_min)
        throughput = counter*3600/ (out_time_tail_max- in_time_min)  
        # throughput = counter*3600/throughput   
        # print(delay)
        f = open("result/500vph/AIMPBSDelay.txt", "a")
        f.write(str(delay))
        f.write("\n")
        f.close()

        f = open("result/500vph/AIMPBSThroughput.txt", "a")
        f.write(str(throughput))
        f.write("\n")
        f.close()





for randomness in range(99):
    print("Iteration " + str(randomness+1))
    os.system("python generate_arrivals_new_500vph.py")
    print("\n\n\n\n AIMPBS")
    for step in range(100):
        commandPBS = "../PBS-SIPP-ver2/AIMPBS " + str(step)
        # os.system(commandPBS)
        time_running = os.popen(commandPBS).read()
        print(time_running[:-1])

        with open('result/500vph/AIMPBSRunTime.txt', 'r') as file:
            # read a list of lines into data
            data = file.readlines()
        data[step] = data[step][:-1] + ";" + time_running[:-1] + "\n"

        with open('result/500vph/AIMPBSRunTime.txt', 'w') as file:
            file.writelines( data )


        # f = open("result/500vph/AIMPBSRunTime.txt", "a")
        # f.write(time_running[:-1])
        # f.write("\n")
        # f.close()

        os.system("cp output.txt output_PBSSIPP.txt")

        with open("output.txt") as f:
            next(f)
            counter = 0

            max_speed = 15
            delay = 0
            throughput = 0
            in_time_min = 10000
            out_time_tail_max = 0
            for line_in_file in f:
                counter+=1
                data = line_in_file.split(";")

                in_point = data[0]
                out_point = data[1]
                length = float(data[2])
                earliest_arrival = float(data[3])
                in_time = float(data[4])
                out_time_head = float(data[5])
                out_time_tail = float(data[6])
                time_spend = float(data[7])
                speed = float(data[8])

                delay += out_time_head - earliest_arrival - length/max_speed

                # if (in_time_min > in_time):
                #     in_time_min = in_time
                # if (out_time_tail_max < out_time_tail ):
                #     out_time_tail_max = out_time_tail

                throughput += out_time_tail - in_time
        delay = delay/counter
        # print(counter, out_time_tail_max, in_time_min)
        # throughput = counter*3600/ (out_time_tail_max- in_time_min)  
        throughput = counter*3600/throughput   
        # print(delay)

        with open('result/500vph/AIMPBSDelay.txt', 'r') as file:
            # read a list of lines into data
            data = file.readlines()
        data[step] = data[step][:-1] + ";" + str(delay) + "\n"

        with open('result/500vph/AIMPBSDelay.txt', 'w') as file:
            file.writelines( data )



        with open('result/500vph/AIMPBSThroughput.txt', 'r') as file:
            # read a list of lines into data
            data = file.readlines()
        data[step] = data[step][:-1] +  ";" + str(throughput) + "\n"
        with open('result/500vph/AIMPBSThroughput.txt', 'w') as file:
            file.writelines( data )







