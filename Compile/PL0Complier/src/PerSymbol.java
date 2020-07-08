/**
 * PerSymbol��
 */
public class PerSymbol {
	private int type;           //��ʾ���������������
    private int value;          //��ʾ�����������ֵ
    private int level;          //Ƕ�ײ��
    private int address;        //���������Ƕ�׹��̻���ַ�ĵ�ַ
    private int size;           //��ʾ������������������ռ�Ĵ�С(��һ����ʵĬ��Ϊ0�� ��û���õ�)
    private String name;        //�����������������

    public PerSymbol(int _type, int _value, int _level, int _address, int _size, String _name) {
    	type = _type;
    	value = _value;
    	level = _level;
    	address = _address;
    	size = _size;
    	name = _name;
    }

    public PerSymbol(int _type, int _level, int _address, int _size, String _name) {
    	//רΪ���������͹�������д�Ĺ��캯��
        //�����͹�������ʱû�г�ʼֵ
    	type = _type;
    	level = _level;
    	address = _address;
    	size = _size;
    	name = _name;
    }

    public void setType(int _type) {
    	type = _type;
    }

    public void setValue(int _value) {
        value = _value;
    }

    public void setLevel(int _level) {
        level = _level;
    }

    public void setAddress(int _address) {
        address = _address;
    }

    public void setSize(int _size) {
        size = _size;
    }

    public void setName(String _name) {
        name = _name;
    }

    public int  getType() {
        return type;
    }

    public int getValue() {
        return value;
    }

    public int getLevel() {
        return level;
    }

    public int getAddress() {
        return address;
    }

    public int getSize() {
        return size;
    }

    public String getName() {
        return name;
    }
}
