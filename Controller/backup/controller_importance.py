import os

for randomness in range(0):
    os.system("python generate_arrivals_new.py")
    print("\n\n\n\n AIMPBS")
    for step in range(9,50,5):
        commandPBS = "../PBS-SIPP-ver2-Priority/AIMPBS " + str(step)
        # os.system(commandPBS)
        time_running = os.popen(commandPBS).read()
        print(time_running[:-1])

        os.system("cp output.txt output_PBSSIPP.txt")

        with open("output.txt") as f:
            next(f)
            counter = 0
            counter_normal = 0
            counter_importance = 0

            max_speed = 15
            delay = 0
            delay_normal = 0
            delay_importance = 0

            counter2 = 0
            counter_normal2 = 0
            counter_importance2 = 0
            delay2 = 0
            delay_normal2 = 0
            delay_importance2 = 0

            throughput = 0
            in_time_min = 10000
            out_time_tail_max = 0
            for line_in_file in f:
                
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

                importance = int(data[9])

                if (importance == 1):
                    counter_normal+=2
                    delay_normal += (out_time_head - earliest_arrival - length/max_speed)*2

                    counter += 2
                    delay += (out_time_head - earliest_arrival - length/max_speed)*2

                    counter_normal2+=1
                    delay_normal2 += (out_time_head - earliest_arrival - length/max_speed)

                    counter2 += 1
                    delay2 += (out_time_head - earliest_arrival - length/max_speed)


                if (importance == 2):
                    counter_importance+=20
                    delay_importance += (out_time_head - earliest_arrival - length/max_speed)*20

                    counter += 20
                    delay += (out_time_head - earliest_arrival - length/max_speed)*20


                    counter_importance2+=1
                    delay_importance2 += (out_time_head - earliest_arrival - length/max_speed)

                    counter2 += 1
                    delay2 += (out_time_head - earliest_arrival - length/max_speed)

                # throughput += out_time_tail - in_time

        f = open("20210730-Priority/800vph/TOTALAIMPBSDelayNormal.txt", "a")
        f.write(str(delay_normal))
        f.write("\n")
        f.close()
        f = open("20210730-Priority/800vph/TOTALAIMPBSDelayImportance.txt", "a")
        f.write(str(delay_importance))
        f.write("\n")
        f.close()

        f = open("20210730-Priority/800vph/TOTALAIMPBSDelay.txt", "a")
        f.write(str(delay))
        f.write("\n")
        f.close()


        delay_normal2 = delay_normal2/counter_normal2
        delay_importance2 = delay_importance2/counter_importance2
        delay2 = delay2/counter2

        f = open("20210730-Priority/800vph/AIMPBSDelayNormal.txt", "a")
        f.write(str(delay_normal2))
        f.write("\n")
        f.close()
        f = open("20210730-Priority/800vph/AIMPBSDelayImportance.txt", "a")
        f.write(str(delay_importance2))
        f.write("\n")
        f.close()

        f = open("20210730-Priority/800vph/AIMPBSDelay.txt", "a")
        f.write(str(delay2))
        f.write("\n")
        f.close()



    print("\n\n\n\n AIMPBS-Priority")
    for step in range(9,50,5):
        commandPBS = "../PBS-SIPP-IMP/AIMPBS " + str(step)
        time_running = os.popen(commandPBS).read()
        print(time_running[:-1])

        os.system("cp output.txt output_PBS-SIPP-IMP.txt")

        with open("output.txt") as f:
            next(f)
            counter = 0
            counter_normal = 0
            counter_importance = 0

            max_speed = 15
            delay = 0
            delay_normal = 0
            delay_importance = 0
            counter2 = 0
            counter_normal2 = 0
            counter_importance2 = 0
            delay2 = 0
            delay_normal2 = 0
            delay_importance2 = 0
            throughput = 0
            in_time_min = 10000
            out_time_tail_max = 0
            for line_in_file in f:
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

                importance = int(data[9])

                if (importance == 1):
                    counter_normal+=2
                    delay_normal += (out_time_head - earliest_arrival - length/max_speed)*2

                    counter += 2
                    delay += (out_time_head - earliest_arrival - length/max_speed)*2

                    counter_normal2+=1
                    delay_normal2 += (out_time_head - earliest_arrival - length/max_speed)

                    counter2 += 1
                    delay2 += (out_time_head - earliest_arrival - length/max_speed)


                if (importance == 2):
                    counter_importance+=20
                    delay_importance += (out_time_head - earliest_arrival - length/max_speed)*20

                    counter += 20
                    delay += (out_time_head - earliest_arrival - length/max_speed)*20


                    counter_importance2+=1
                    delay_importance2 += (out_time_head - earliest_arrival - length/max_speed)

                    counter2 += 1
                    delay2 += (out_time_head - earliest_arrival - length/max_speed)
                # throughput += out_time_tail - in_time

        f = open("20210730-Priority/800vph/TOTALAIMPBS-PriorityDelayNormal.txt", "a")
        f.write(str(delay_normal))
        f.write("\n")
        f.close()
        f = open("20210730-Priority/800vph/TOTALAIMPBS-PriorityDelayImportance.txt", "a")
        f.write(str(delay_importance))
        f.write("\n")
        f.close()


        f = open("20210730-Priority/800vph/TOTALAIMPBS-PriorityDelay.txt", "a")
        f.write(str(delay))
        f.write("\n")
        f.close()


        delay_normal2 = delay_normal2/counter_normal2
        delay_importance2 = delay_importance2/counter_importance2
        delay2 = delay2/counter2

        f = open("20210730-Priority/800vph/AIMPBS-PriorityDelayNormal.txt", "a")
        f.write(str(delay_normal2))
        f.write("\n")
        f.close()
        f = open("20210730-Priority/800vph/AIMPBS-PriorityDelayImportance.txt", "a")
        f.write(str(delay_importance2))
        f.write("\n")
        f.close()


        f = open("20210730-Priority/800vph/AIMPBS-PriorityDelay.txt", "a")
        f.write(str(delay2))
        f.write("\n")
        f.close()





for randomness in range(99):
    os.system("python generate_arrivals_new.py")
    print("\n\n\n\n AIMPBS")
    for step in range(9,50,5):
        commandPBS = "../PBS-SIPP-ver2-Priority/AIMPBS " + str(step)
        # os.system(commandPBS)
        time_running = os.popen(commandPBS).read()
        print(time_running[:-1])


        os.system("cp output.txt output_PBSSIPP.txt")

        with open("output.txt") as f:
            next(f)
            counter = 0
            counter_normal = 0
            counter_importance = 0

            max_speed = 15
            delay = 0
            delay_normal = 0
            delay_importance = 0
            counter2 = 0
            counter_normal2 = 0
            counter_importance2 = 0
            delay2 = 0
            delay_normal2 = 0
            delay_importance2 = 0
            throughput = 0
            in_time_min = 10000
            out_time_tail_max = 0
            for line_in_file in f:
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

                importance = int(data[9])

                if (importance == 1):
                    counter_normal+=2
                    delay_normal += (out_time_head - earliest_arrival - length/max_speed)*2

                    counter += 2
                    delay += (out_time_head - earliest_arrival - length/max_speed)*2

                    counter_normal2+=1
                    delay_normal2 += (out_time_head - earliest_arrival - length/max_speed)

                    counter2 += 1
                    delay2 += (out_time_head - earliest_arrival - length/max_speed)


                if (importance == 2):
                    counter_importance+=20
                    delay_importance += (out_time_head - earliest_arrival - length/max_speed)*20

                    counter += 20
                    delay += (out_time_head - earliest_arrival - length/max_speed)*20


                    counter_importance2+=1
                    delay_importance2 += (out_time_head - earliest_arrival - length/max_speed)

                    counter2 += 1
                    delay2 += (out_time_head - earliest_arrival - length/max_speed)
        with open('20210730-Priority/800vph/TOTALAIMPBSDelayNormal.txt', 'r') as file:
            # read a list of lines into data
            data = file.readlines()
        data[int((float(step)+1)/5)-2] = data[int((float(step)+1)/5)-2][:-1] + ";" + str(delay_normal) + "\n"

        with open('20210730-Priority/800vph/TOTALAIMPBSDelayNormal.txt', 'w') as file:
            file.writelines( data )

        delay_normal2 = delay_normal2/counter_normal2
        with open('20210730-Priority/800vph/AIMPBSDelayNormal.txt', 'r') as file:
            # read a list of lines into data
            data = file.readlines()
        data[int((float(step)+1)/5)-2] = data[int((float(step)+1)/5)-2][:-1] + ";" + str(delay_normal2) + "\n"

        with open('20210730-Priority/800vph/AIMPBSDelayNormal.txt', 'w') as file:
            file.writelines( data )




        if (counter_importance >0):
            with open('20210730-Priority/800vph/TOTALAIMPBSDelayImportance.txt', 'r') as file:
                # read a list of lines into data
                data = file.readlines()
            data[int((float(step)+1)/5)-2] = data[int((float(step)+1)/5)-2][:-1] + ";" + str(delay_importance) + "\n"

            with open('20210730-Priority/800vph/TOTALAIMPBSDelayImportance.txt', 'w') as file:
                file.writelines( data )
            delay_importance2 = delay_importance2/counter_importance2
            with open('20210730-Priority/800vph/AIMPBSDelayImportance.txt', 'r') as file:
                # read a list of lines into data
                data = file.readlines()
            data[int((float(step)+1)/5)-2] = data[int((float(step)+1)/5)-2][:-1] + ";" + str(delay_importance2) + "\n"

            with open('20210730-Priority/800vph/AIMPBSDelayImportance.txt', 'w') as file:
                file.writelines( data )





        with open('20210730-Priority/800vph/TOTALAIMPBSDelay.txt', 'r') as file:
            # read a list of lines into data
            data = file.readlines()
        data[int((float(step)+1)/5)-2] = data[int((float(step)+1)/5)-2][:-1] + ";" + str(delay) + "\n"

        with open('20210730-Priority/800vph/TOTALAIMPBSDelay.txt', 'w') as file:
            file.writelines( data )

        delay2 = delay2/counter2
        with open('20210730-Priority/800vph/AIMPBSDelay.txt', 'r') as file:
            # read a list of lines into data
            data = file.readlines()
        data[int((float(step)+1)/5)-2] = data[int((float(step)+1)/5)-2][:-1] + ";" + str(delay2) + "\n"

        with open('20210730-Priority/800vph/AIMPBSDelay.txt', 'w') as file:
            file.writelines( data )




    print("\n\n\n\n AIMPBS-Priority")
    for step in range(9,50,5):
        commandPBS = "../PBS-SIPP-IMP/AIMPBS " + str(step)
        time_running = os.popen(commandPBS).read()
        print(time_running[:-1])

        os.system("cp output.txt output_PBS-SIPP-IMP.txt")

        with open("output.txt") as f:
            next(f)
            counter = 0
            counter_normal = 0
            counter_importance = 0

            max_speed = 15
            delay = 0
            delay_normal = 0
            delay_importance = 0
            counter2 = 0
            counter_normal2 = 0
            counter_importance2 = 0
            delay2 = 0
            delay_normal2 = 0
            delay_importance2 = 0
            throughput = 0
            in_time_min = 10000
            out_time_tail_max = 0
            for line_in_file in f:
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

                importance = int(data[9])

                if (importance == 1):
                    counter_normal+=2
                    delay_normal += (out_time_head - earliest_arrival - length/max_speed)*2

                    counter += 2
                    delay += (out_time_head - earliest_arrival - length/max_speed)*2

                    counter_normal2+=1
                    delay_normal2 += (out_time_head - earliest_arrival - length/max_speed)

                    counter2 += 1
                    delay2 += (out_time_head - earliest_arrival - length/max_speed)


                if (importance == 2):
                    counter_importance+=20
                    delay_importance += (out_time_head - earliest_arrival - length/max_speed)*20

                    counter += 20
                    delay += (out_time_head - earliest_arrival - length/max_speed)*20


                    counter_importance2+=1
                    delay_importance2 += (out_time_head - earliest_arrival - length/max_speed)

                    counter2 += 1
                    delay2 += (out_time_head - earliest_arrival - length/max_speed)
        with open('20210730-Priority/800vph/TOTALAIMPBS-PriorityDelayNormal.txt', 'r') as file:
            # read a list of lines into data
            data = file.readlines()
        data[int((float(step)+1)/5)-2] = data[int((float(step)+1)/5)-2][:-1] + ";" + str(delay_normal) + "\n"

        with open('20210730-Priority/800vph/TOTALAIMPBS-PriorityDelayNormal.txt', 'w') as file:
            file.writelines( data )
        delay_normal2 = delay_normal2/counter_normal2
        with open('20210730-Priority/800vph/AIMPBS-PriorityDelayNormal.txt', 'r') as file:
            # read a list of lines into data
            data = file.readlines()
        data[int((float(step)+1)/5)-2] = data[int((float(step)+1)/5)-2][:-1] + ";" + str(delay_normal2) + "\n"

        with open('20210730-Priority/800vph/AIMPBS-PriorityDelayNormal.txt', 'w') as file:
            file.writelines( data )





        if (counter_importance >0):
            with open('20210730-Priority/800vph/TOTALAIMPBS-PriorityDelayImportance.txt', 'r') as file:
                # read a list of lines into data
                data = file.readlines()
            data[int((float(step)+1)/5)-2] = data[int((float(step)+1)/5)-2][:-1] + ";" + str(delay_importance) + "\n"

            with open('20210730-Priority/800vph/TOTALAIMPBS-PriorityDelayImportance.txt', 'w') as file:
                file.writelines( data )
            delay_importance2 = delay_importance2/counter_importance2
            with open('20210730-Priority/800vph/AIMPBS-PriorityDelayImportance.txt', 'r') as file:
                # read a list of lines into data
                data = file.readlines()
            data[int((float(step)+1)/5)-2] = data[int((float(step)+1)/5)-2][:-1] + ";" + str(delay_importance2) + "\n"

            with open('20210730-Priority/800vph/AIMPBS-PriorityDelayImportance.txt', 'w') as file:
                file.writelines( data )





        with open('20210730-Priority/800vph/TOTALAIMPBS-PriorityDelay.txt', 'r') as file:
            # read a list of lines into data
            data = file.readlines()
        data[int((float(step)+1)/5)-2] = data[int((float(step)+1)/5)-2][:-1] + ";" + str(delay) + "\n"

        with open('20210730-Priority/800vph/TOTALAIMPBS-PriorityDelay.txt', 'w') as file:
            file.writelines( data )

        delay2 = delay2/counter2
        with open('20210730-Priority/800vph/AIMPBS-PriorityDelay.txt', 'r') as file:
            # read a list of lines into data
            data = file.readlines()
        data[int((float(step)+1)/5)-2] = data[int((float(step)+1)/5)-2][:-1] + ";" + str(delay2) + "\n"

        with open('20210730-Priority/800vph/AIMPBS-PriorityDelay.txt', 'w') as file:
            file.writelines( data )
