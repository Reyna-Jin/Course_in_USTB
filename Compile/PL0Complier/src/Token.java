/**
 *token��
 */
public class Token {
	private SymType st; //token�����
	private int line; //token�����У�������ʹ��
	private String value; //token��ֵ��ֻ�б�ʶ���ͳ�����ֵ

	public Token(SymType _st, int _line, String _value) {
		st = _st;
		line = _line;
		value = _value;
	}

	public void setSt(SymType _st) {
		st = _st;
	}

	public void setLine(int _line) {
		line = _line;
	}

	public void setValue(String _value) {
		value = _value;
	}

	public SymType getSt() {
		return st;
	}

	public int getLine() {
		return line;
	}

	public String getValue() {
		return value;
	}
}
