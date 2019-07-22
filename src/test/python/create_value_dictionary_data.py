import argparse
import struct

class ValueDictionary:
    VALUE_FMT_CODE = { 1 : 'B', 2 : 'H', 4 : 'I', 8 : 'Q' }
    
    def __init__(self, keys_and_values, value_size, value_alignment):
        self.value_for_key = keys_and_values
        self.value_size = value_size
        self.value_alignment = value_alignment
        try:
            self._value_fmt_code = '<' + self.VALUE_FMT_CODE[value_size]
        except IndexError:
            raise ValueError('value_size must be 1, 2, 4 or 8')
        
    def write_bytes(self, output):
        (buffer, dict_size) = self._create()
        output.write(buffer)
        return dict_size

    def write_cpp(self, output, var_name):
        (buffer, dict_size) = self._create()
        output.write('static const uint8_t %s[] = {' % var_name)

        for i in xrange(0, len(buffer)):
            if not (i % 12):
                output.write('\n   ')
            output.write(' 0x%02x,' % buffer[i])
        output.write('\n};')
        
    def _create(self):
        keys = sorted(self.value_for_key)
        dict_size = self._compute_dict_size(keys)
        print 'dict_size = %d' % dict_size
        buffer = bytearray(dict_size)
        struct.pack_into('<i', buffer, 0, len(keys))
        if not keys:
            output.write(buffer)
            return

        # Write offsets to last byte of each key
        o = 4
        e = -1
        for key in keys:
            e += len(key)
            struct.pack_into('<I', buffer, o, e)
            o += 4

        # Padding to align values
        padding = self._align_size(o, self.value_alignment) - o
        if padding:
            struct.pack_into('<%dB' % padding, buffer, o, 0)
            o += padding

        # Write values
        for key in keys:
            struct.pack_into(self._value_fmt_code, buffer, o,
                             self.value_for_key[key])
            o += self.value_size

        # Write keys
        for key in keys:
            struct.pack_into('<%ds' % len(key), buffer, o, key)
            o += len(key)

        # Pad to a multiple of four bytes
        padding = self._align_size(o, 4) - o
        if padding:
            tmp = (0, ) * padding
            struct.pack_into('<%dB' % padding, buffer, o, *tmp)
            o += padding

        assert o == dict_size
        return (buffer, dict_size)
        
    def _compute_dict_size(self, keys):
        dict_size = self._align_size(4 + (4 * len(keys)), self.value_alignment)
        dict_size += self.value_size * len(keys)
        dict_size += sum(len(k) for k in keys)
        return self._align_size(dict_size, 4)

    def _align_size(self, value, alignment):
        return (value + alignment - 1) & ~(alignment - 1)

def read_kv_file(filename):
    d = { }
    with open(filename, 'r') as input:
        for (line, text) in enumerate(input):
            text = text.strip(' ').rstrip(' \r\n')
            if text:
                fields = text.split('\t')
                if len(fields) == 1:
                    error(line + 1, 'Value missing')
                elif len(fields) > 2:
                    error(line + 1, 'Too many columns')
                key = fields[0].strip()
                if not key:
                    error(line + 1, 'Key is empty')
                value = fields[1].strip()
                if not value:
                    error(line + 1, 'Value is missing')
                try:
                    value = int(value)
                except ValueError:
                    error(line + 1, 'Value must be an integer')
                d[key] = value
    return d
        
if __name__ == '__main__':
    parser = argparse.ArgumentParser(
        description = 'Write a ValueDictionary to a flat file or C++ source ' +\
                      'code for unit testing')
    parser.add_argument('--cpp', dest = 'cpp_var_name', action = 'store',
                        help = 'Write the dictionary as a C++ source file, ' + \
                               'using the given variable name')
    parser.add_argument('-o', dest = 'output_filename', action = 'store',
                        help = 'Output file name', required = True)
    parser.add_argument('--value-size', dest = 'value_size', action = 'store',
                        type = int, default = 4,
                        help = 'Size of value (default = 4)')
    parser.add_argument('--value-alignment', dest = 'value_alignment',
                        action = 'store', type = int, default = 4,
                        help = 'Alignment of value (default = 4)')
    parser.add_argument('input_filename',
                        help = 'File with list of dictionary keys and values')

    args = parser.parse_args()
    keys_and_values = read_kv_file(args.input_filename)
    value_dict = ValueDictionary(keys_and_values, args.value_size,
                                 args.value_alignment)

    with open(args.output_filename, 'w') as output:
        if hasattr(args, 'cpp_var_name') and args.cpp_var_name:
            value_dict.write_cpp(output, args.cpp_var_name)
        else:
            value_dict.write_bytes(output)
    
