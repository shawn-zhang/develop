public class SerialPort {

	public enum Parity {
		None(0),
		Odd(1),
		Even(2),
		Mark(3),
		Space(4);
		private int value;
		private Parity(int value) {

			this.value = value;

		}
		public int value() {
			return this.value;
		}
	}

	public native boolean setParity(Parity parity);
	public native Parity getParity();
}
