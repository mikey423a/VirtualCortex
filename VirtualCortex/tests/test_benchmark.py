import pathlib
import re
import subprocess
import sys
import tempfile

bench = sys.argv[1]
with tempfile.TemporaryDirectory() as directory:
    path = pathlib.Path(directory) / 'test.vcmlp'
    # Eight inputs per neuron, eight biases, eight output weights, output bias.
    path.write_text('VCMLP1\n' + ' '.join(['0.1'] * 64 + ['0'] * 8 + ['0.1'] * 8 + ['0']) + '\n')
    result = subprocess.run([bench, str(path), '1', '2'], capture_output=True, text=True)
    assert result.returncode == 0, result.stderr
    assert 'instances=2' in result.stdout
    assert float(re.search(r'realtime_factor=([\d.]+)', result.stdout).group(1)) > 0
    assert subprocess.run([bench, str(path), '0'], capture_output=True).returncode != 0
    assert subprocess.run([bench, str(path), '1', '33'], capture_output=True).returncode != 0
print('benchmark smoke checks passed')
