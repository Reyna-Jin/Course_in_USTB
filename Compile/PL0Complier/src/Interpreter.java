import java.util.ArrayList;
import java.util.List;
import java.util.Scanner;

public class Interpreter {
	private int STACK_SIZE = 1000;
	private int[] dataStack = new int[STACK_SIZE];
	private List<PerPcode> pcode;
	private List<Integer> input;
	private int inputPtr;
	private List <String> output;

	public Interpreter() {
	
	}
	
    public Interpreter(List<Integer> _input) {
        input = _input;
        output = new ArrayList<String>();
	}

	public void setAllPcode(AllPcode allPcode) {
		pcode = allPcode.getAllPcode();
	}

    public List<String> getOutput() {
        return output;
    }

	public void interpreter() {
		int pc = 0; //�����������ָ����һ��ָ��
		int base = 0; //��ǰ����ַ
		int top = 0; //��������ջջ��
		do {
			PerPcode currentPcode = pcode.get(pc);
			pc++;
			if (currentPcode.getF() == Operator.LIT) {
                        //LIT���������͵�����ջS��ջ������ʱA��Ϊ����ֵ
				dataStack[top] = currentPcode.getA();
				top++;
			} else if (currentPcode.getF() == Operator.OPR) {
                        //OPR����ϵ���������㣬A��ָ����������
				switch(currentPcode.getA()) {
				case 0:
                              //OPR 0 0   ���̵��ý�����,���ص��õ㲢��ջ
					top = base;
                              pc = dataStack[base + 2];
                              base = dataStack[base];
                              break;
                   	case 1:
                              //OPR 0 1     ջ��Ԫ��ȡ��
                   		dataStack[top - 1] = -dataStack[top - 1];
                              break;
                   	case 2:
                              //OPR 0 2   ��ջ����ջ����ӣ�������ջԪ�أ����ֵ��ջ
                   		dataStack[top - 2] = dataStack[top - 1] + dataStack[top - 2];
                   		top--;
                   		break;
                   	case 3:
                              //OPR 0 3   ��ջ����ȥջ����������ջԪ�أ����ֵ��ջ
                   		dataStack[top - 2] = dataStack[top - 2] - dataStack[top - 1];
                   		top--;
                   		break;
                   	case 4:
                              //OPR 0 4     ��ջ������ջ����������ջԪ�أ����ֵ��ջ
                   		dataStack[top - 2] = dataStack[top - 1] * dataStack[top - 2];
                   		top--;
                   		break;
                   	case 5:
                              //OPR 0 5     ��ջ������ջ����������ջԪ�أ����ֵ��ջ
                   		dataStack[top - 2] = dataStack[top - 2] / dataStack[top - 1];
                   		top--;
                   		break;
                   	case 6:
                              //OPR 0 6   ջ��Ԫ�ص���ż�жϣ����ֵ��ջ��
                   		dataStack[top - 1] = dataStack[top - 1] % 2;
                   		break;
                   	case 7:
                   		break;
                   	case 8:
                              //OPR 0 8   ��ջ����ջ���Ƿ���ȣ�������ջԪ�أ����ֵ��ջ
                   		if (dataStack[top - 2] == dataStack[top - 1]) {
                   			dataStack[top - 2] = 1;
                   		} else {
                   			dataStack[top - 2] = 0;
                   		}
                   		top--;
                   		break;
                   	case 9:
                              //OPR 0 9   ��ջ����ջ���Ƿ񲻵ȣ�������ջԪ�أ����ֵ��ջ
                   		if (dataStack[top - 2] != dataStack[top - 1]) {
                   			dataStack[top - 2] = 1;
                   		} else {
                   			dataStack[top - 2] = 0;
                   		}
                   		top--;
                   		break;
                   	case 10:
                              //OPR 0 10  ��ջ���Ƿ�С��ջ����������ջԪ�أ����ֵ��ջ
                   		if (dataStack[top - 2] < dataStack[top - 1]) {
                   			dataStack[top - 2] = 1;
                   		} else {
                   			dataStack[top - 2] = 0;
                   		}
                   		top--;
                   		break;
                   	case 11:
                              //OPR 0 11    ��ջ���Ƿ���ڵ���ջ����������ջԪ�أ����ֵ��ջ
                   		if (dataStack[top - 2] >= dataStack[top - 1]) {
                   			dataStack[top - 2] = 1;
                   		} else {
                   			dataStack[top - 2] = 0;
                   		}
                   		top--;
                   		break;
                   	case 12:
                              //OPR 0 12  ��ջ���Ƿ����ջ����������ջԪ�أ����ֵ��ջ
                   		if (dataStack[top - 2] > dataStack[top - 1]) {
                   			dataStack[top - 2] = 1;
                   		} else {
                   			dataStack[top - 2] = 0;
                   		}
                   		top--;
                   		break;
                   	case 13:
                              //OPR 0 13  ��ջ���Ƿ�С�ڵ���ջ����������ջԪ�أ����ֵ��ջ
                   		if (dataStack[top - 2] <= dataStack[top - 1]) {
                   			dataStack[top - 2] = 1;
                   		} else {
                   			dataStack[top - 2] = 0;
                   		}
                   		top--;
                   		break;
                   	case 14:
                              //OPR 0 14  ջ��ֵ�������Ļ
                   		System.out.print(dataStack[top - 1]);
                   		System.out.print(" ");
                              //output.add(dataStack[top - 1] + " ");
                   		break;
                   	case 15:
                              //OPR 0 15  ��Ļ�������
                   		System.out.println();
                              //output.add("\n");
                   		break;
                   	case 16:
                              //OPR 0 16  �������ж���һ����������ջ��
                              System.out.println("please input a number");
                   		Scanner s = new Scanner(System.in);
                   		dataStack[top] = s.nextInt();
                              //dataStack[top] = input.get(inputPtr++);
                   		top++;
                   		break;
				}
			} else if (currentPcode.getF() == Operator.LOD) {
                        // LOD���������͵�����ջS��ջ������ʱA��Ϊ��������˵�����е����λ�á�
				dataStack[top] = dataStack[currentPcode.getA() + getBase(base, currentPcode.getL())];
				top++;
			} else if (currentPcode.getF() == Operator.STO) {
                        //STO��������ջS��ջ����������ĳ��������Ԫ�У�A��Ϊ��������˵�����е����λ�á�
				dataStack[currentPcode.getA() + getBase(base, currentPcode.getL())] = dataStack[top - 1];
				top--;
			} else if (currentPcode.getF() == Operator.CAL) {
                        //CAL�����ù��̣���ʱA��Ϊ�����ù��̵Ĺ����壨������֮ǰһ��ָ���Ŀ�����������ڵ�ַ��
                        //��תʱ�����ò����ַ����ת�����ַ��pcָ�뱣����ջ��
                        //����ַbase��Ϊ��ʱջ��top��pcָ��Ҫ��ת�ĵط�
                        //���޸�top����Ϊǰ������Ѿ���address+3������Pcode������INT��䣬�޸�topֵ
				dataStack[top] = base;
				dataStack[top + 1] = getBase(base, currentPcode.getL());
				dataStack[top + 2] = pc;
				base = top;
				pc = currentPcode.getA();
			} else if (currentPcode.getF() == Operator.INT) {
                        //INT��Ϊ�����õĹ��̣����������̣�������ջS�п�������������ʱA��Ϊ�������ݵ�Ԫ���������������������ݣ���L�κ�Ϊ0��
				top = top + currentPcode.getA();
			} else if (currentPcode.getF() == Operator.JMP) {
                        //JMP��������ת�ƣ���ʱA��Ϊת���ַ��Ŀ����򣩡�
				pc = currentPcode.getA();
			} else if (currentPcode.getF() == Operator.JPC) {
                        //JPC������ת�ƣ�������ջS��ջ���Ĳ���ֵΪ�٣�0��ʱ����ת��A����ָĿ������ַ������˳��ִ�С�
				if (dataStack[top - 1] == 0) {
					pc = currentPcode.getA();
				}
			}
			//System.out.println(pc + " " + base + " " + top);
		} while (pc != 0);
	}

      //��֪�ò����ַΪnowBp,�õ����Ϊlev�Ĳ�Ļ���ַ
      private int getBase(int nowBp,int lev) { 
        int oldBp = nowBp;      
        while (lev > 0) {
            oldBp = dataStack[oldBp + 1];
            lev--;
        }
        return oldBp;
      }
}
