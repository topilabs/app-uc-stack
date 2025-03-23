var cobs = require('..');

describe('COBS Encoding and Decoding', () => {
    function check(buf) {
        const encoded = cobs.encode(buf);
        const decoded = cobs.decode(encoded);
        expect(decoded).toEqual(buf);
    }

    test('should encode and decode buffer with single zero byte', () => {
        check(Buffer.from([0x00]));
    });

    test('should encode and decode buffer with zero byte in the middle', () => {
        check(Buffer.from([0x11, 0x22, 0x00, 0x33]));
    });

    test('should encode and decode buffer with multiple zero bytes', () => {
        check(Buffer.from([0x11, 0x00, 0x00, 0x00]));
    });

    test('should encode and decode buffer with range of bytes', () => {
        const range = Array.from({ length: 255 - 1 }, (_, i) => i + 1);
        check(Buffer.from(range));
    });
});