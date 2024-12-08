
# This is for quantized model 
quant_file_path="quant_file_1.tflite"

# This is for non-quantied model 
float_file_path="float_file_1.tflite"

with open(quant_file_path, 'rb') as file:
    # Read the binary data from the file
    binary_data = file.read()

# print(binary_data)

file = open("mnist_quant"+".tflite.cc", 'w') 


file.write(

   "#include \"BufAttributes.hpp\"\n"
    "#include \"../../../ra/fsp/src/bsp/mcu/all/bsp_compiler_support.h\"\n"
    "#include <cstddef>\n"
    "#include <cstdint>\n"
    "#include \"../../conf.h\"\n\n"
    "namespace arm {\n"
    "namespace app {\n"
    "namespace object_detection {\n\n"
    "extern const int originalImageSize = 144;\n\n"
    "#if QUANT==1\n\n"

)

file.write("static const uint8_t nn_model[] MODEL_TFLITE_ATTRIBUTE ={\n")
hex_string = ", ".join(["0x{:02x}".format(byte) for byte in binary_data])
file.write(hex_string)
# for i in range(len(binary_data)):
#     if i%12==0:
#         file.write("\n")
    
#     file.write(str(binary_data[i]) + ", ")

file.write("\n};\n\n")

file.write("#else\n\n")

with open(float_file_path, 'rb') as file1:
    # Read the binary data from the file
    binary_data = file1.read()


file.write("static const uint8_t nn_model[] MODEL_TFLITE_ATTRIBUTE ={\n")
hex_string = ", ".join(["0x{:02x}".format(byte) for byte in binary_data])

file.write(hex_string)
file.write("\n};\n\n")
file.write("#endif\n\n")
file.write(


        "const uint8_t * GetModelPointer()\n"
        "{\n"
        "    return nn_model;\n"
        "}\n\n"
        "size_t GetModelLen()\n"
        "{\n"
        "    return sizeof(nn_model);\n"
        "}\n\n"

        "} /* namespace arm */\n"
        "} /* namespace app */\n"
        "} /* namespace object_detection */\n"


)


file.close()
