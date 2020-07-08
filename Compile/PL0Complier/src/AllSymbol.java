import java.util.ArrayList;
import java.util.List;

/**
 * ���ű�
 */
public class AllSymbol {
	List<PerSymbol> allSymbol;

    private int con=1;              //����������1��ʾ
    private int var=2;              //����������2��ʾ
    private int proc=3;             //����������3��ʾ

    private int ptr = 0;

	public AllSymbol() {
		allSymbol = new ArrayList<PerSymbol>();
	}

	//����ű��в��볣��
	public void enterConst(String name, int level, int value, int address) {
        allSymbol.add(new PerSymbol(con, value, level, address, 0, name));
        ptr++;
    }

    //����ű��в������
    public void enterVar(String name, int level, int address) {
    	allSymbol.add(new PerSymbol(var, level, address, 0, name));
        ptr++;
    }

    //����ű��в������
    public void enterProc(String name, int level, int address) {
    	allSymbol.add(new PerSymbol(proc, level, address, 0, name));
        ptr++;
    }

    //�ڷ��ű�ǰ����ұ����Ƿ����
    public boolean isNowExists(String name, int level) {
    	for (int i = 0; i < allSymbol.size(); i++) {
    		if (allSymbol.get(i).getName().equals(name) && allSymbol.get(i).getLevel() == level) {
    			return true;
    		}
    	}
    	return false;
    }

    //�ڷ��ű�֮ǰ����ҷ����Ƿ����
    public boolean isPreExists(String name, int level) {
    	for (int i = 0; i < allSymbol.size(); i++) {
    		if (allSymbol.get(i).getName().equals(name) && allSymbol.get(i).getLevel() <= level) {
    			return true;
    		}
    	}
    	return false;
    }

    //�����Ʋ��ұ���
    public PerSymbol getSymbol(String name) {
    	for (int i = allSymbol.size() - 1; i >= 0; i--) {
    		if (allSymbol.get(i).getName().equals(name)) {
    			return allSymbol.get(i);
    		}
    	}
    	return null;
    }

    //���ҵ�ǰ�����ڵĹ���
    public int getLevelProc(int level) {
    	for (int i = allSymbol.size() - 1; i >= 0; i--) {
    		if (allSymbol.get(i).getType() == proc) {
    			return i;
    		}
    	}
    	return -1;
    }

    public List<PerSymbol> getAllSymbol() {
        return allSymbol;
    }

    public void setPtr(int _ptr) {
        ptr = _ptr;
    }

    public int getPtr() {
        return ptr;
    }

	public int getLength() {
		return allSymbol.size();
	}

	public int getCon() {
		return con;
	}

	public int getVar() {
		return var;
	}

	public int getProc() {
		return proc;
	}
}
