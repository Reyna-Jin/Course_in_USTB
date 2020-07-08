import java.io.File;
import java.util.ArrayList;
import java.util.List;

/**
 * �﷨����+������������ɷ��ű��Pcode��
 * ������
 */
public class GSAnalysis {
	private LexAnalysis lex;
	private List<Token> allToken; //����ʷ��������
	private AllPcode allPcode; //�������ɵ�Pcode
	private AllSymbol allSymbol; //���ű����
	private List<String> errorMessage; //���������Ϣ

	private boolean errorHappen = false; //��¼����������Ƿ�������
	private int tokenPtr = 0; //ָ��ǰtoken��ָ��

	private int level = 0;
	private int address = 0;
	private int addIncrement = 1;
	
	GSAnalysis(File file) {
		lex = new LexAnalysis(file);
		allToken = lex.getAllToken();

		allPcode = new AllPcode();

		allSymbol = new AllSymbol();
		
		errorMessage = new ArrayList<String>();
	}

	public boolean compile() {
		program();
		return (!errorHappen);
	}

	private void program() {
		//<������>::=<�ֳ���>.
		block();
		if (allToken.get(tokenPtr).getSt() == SymType.POI) {
			tokenPtr++;
			if (allToken.get(tokenPtr).getSt() != SymType.EOF) {
				errorHandle(18, "");
			}
		} else {
			errorHandle(17, "");
		}
	}

	private void block() {
		//<�ֳ���>::=[<����˵������>][<����˵������>][<����˵������>]<���>
		int address_cp = address; //����֮ǰ��addressֵ
		
		//��ʼ���������ر���
		int start = allSymbol.getPtr(); //������������ĳ�ʼλ��
		int pos = 0; //������������ڷ��ű��е�λ��
		address = 3; //Ĭ����3��ʼ��ǰ��λ���һЩ��ת�ؼ���������ԭ����base������ַ����pc���������������
		if (start > 0) {
			pos = allSymbol.getLevelProc(level);
		}

		//������תָ������������֣��������
		int tmpPcodePtr = allPcode.getPcodePtr(); 
		allPcode.gen(Operator.JMP, 0, 0);

		if (allToken.get(tokenPtr).getSt() == SymType.CON) {
			conDeclare();
		}
		if (allToken.get(tokenPtr).getSt() == SymType.VAR) {
			varDeclare();
		}
		if (allToken.get(tokenPtr).getSt() == SymType.PROC) {
			proc();
		}

		allPcode.getAllPcode().get(tmpPcodePtr).setA(allPcode.getPcodePtr()); //������ת��ַ
		allPcode.gen(Operator.INT, 0, address); //����ռ�
		if (start != 0) {
			//�������������������Ҫ�ڷ��ű��е�value����ù�����Pcode�����е���ʼλ��
			allSymbol.getAllSymbol().get(pos).setValue(allPcode.getPcodePtr() - 1 - allSymbol.getAllSymbol().get(pos).getSize());
		}

		statement();
		allPcode.gen(Operator.OPR, 0, 0); //���̽���

		address = address_cp;
	}

	private void conDeclare() {
		//<����˵������>::=const <��������>{,<��������>}
		if (allToken.get(tokenPtr).getSt() == SymType.CON) {
			tokenPtr++;
			conHandle();
			while (allToken.get(tokenPtr).getSt() == SymType.COMMA || allToken.get(tokenPtr).getSt() == SymType.SYM) {
				if (allToken.get(tokenPtr).getSt() == SymType.COMMA) {
					tokenPtr++;
				} else {
					errorHandle(23, "");
				}
				conHandle();
			}
			if (allToken.get(tokenPtr).getSt() == SymType.SEMIC) {
				tokenPtr++;
			} else { //ȱ�٣�
				errorHandle(0, "");
			}
		} else { //ȱ��const
			errorHandle(-1, "");
		}
	}

	private void conHandle() {
		//<��������>::=<��ʶ��>=<�޷�������>
		String name;
		int value;
		if (allToken.get(tokenPtr).getSt() == SymType.SYM) {
			name = allToken.get(tokenPtr).getValue();
			tokenPtr++;
			if (allToken.get(tokenPtr).getSt() == SymType.EQU || allToken.get(tokenPtr).getSt() == SymType.CEQU) {
				if (allToken.get(tokenPtr).getSt() == SymType.CEQU) {
					errorHandle(3, "");
				}
				tokenPtr++;
				if (allToken.get(tokenPtr).getSt() == SymType.CONST) {
					value = Integer.parseInt(allToken.get(tokenPtr).getValue());
					if (allSymbol.isNowExists(name, level)) {
						errorHandle(15, name);
					}
					allSymbol.enterConst(name, level, value, address);
					//address += addIncrement;
					tokenPtr++;
				}
			} else { //��ֵû��=
				errorHandle(3, "");
			}
		} else { //��ʶ�����Ϸ�
			errorHandle(1, "");
		}
	}

	private void varDeclare() {
		//<����˵������>::=var<��ʶ��>{,<��ʶ��>}
		String name;
		int value;
		if (allToken.get(tokenPtr).getSt() == SymType.VAR) {
			tokenPtr++;
			if (allToken.get(tokenPtr).getSt() == SymType.SYM) {
				name = allToken.get(tokenPtr).getValue();
				if (allSymbol.isNowExists(name, address)) {
					errorHandle(15, name);
				}
				allSymbol.enterVar(name, level, address);
				address += addIncrement;
				tokenPtr++;
				while (allToken.get(tokenPtr).getSt() == SymType.COMMA || allToken.get(tokenPtr).getSt() == SymType.SYM) {
					if (allToken.get(tokenPtr).getSt() == SymType.COMMA) {
						tokenPtr++;
					} else {
						errorHandle(23, "");
					}
					if (allToken.get(tokenPtr).getSt() == SymType.SYM) {
						name = allToken.get(tokenPtr).getValue();
						if (allSymbol.isNowExists(name, address)) {
							errorHandle(15, name);
						}
						allSymbol.enterVar(name, level, address);
						address += addIncrement;
						tokenPtr++;
					} else { //�Ƿ���ʶ��
						errorHandle(1, "");
						return;
					}
				}
				if (allToken.get(tokenPtr).getSt() != SymType.SEMIC) { //ȱ�٣�
					errorHandle(0, "");
					return;
				} else {
					tokenPtr++;
				}
			} else { //�Ƿ���ʶ��
				errorHandle(1, "");
				return;
			}
		} else { //ȱ��var
			errorHandle(-1, "");
			return;
		}
	}

	private void proc() {
		//<����˵������>::=<�����ײ�><�ֳ���>{;<����˵������>};
		//<�����ײ�>::=procedure<��ʶ��>;
		if (allToken.get(tokenPtr).getSt() == SymType.PROC) {
			tokenPtr++;
			int count = 0; //��¼��������
			int pos; //��¼�ù����ڷ��ű��е�λ��
			if (allToken.get(tokenPtr).getSt() == SymType.SYM) {
				String name = allToken.get(tokenPtr).getValue();
				if (allSymbol.isNowExists(name, level)) {
					errorHandle(15, name);
				}
				pos = allSymbol.getPtr();
				allSymbol.enterProc(name, level, address);
				address += addIncrement;
				level++;
				tokenPtr++;
				
				if (allToken.get(tokenPtr).getSt() == SymType.SEMIC) {
					tokenPtr++;
				} else {
					errorHandle(0, "");
				}
				block();
				while (allToken.get(tokenPtr).getSt() == SymType.SEMIC || allToken.get(tokenPtr).getSt() == SymType.PROC) {
					if (allToken.get(tokenPtr).getSt() == SymType.SEMIC) {
						tokenPtr++;
					} else {
						errorHandle(0, "");
					}
					level--;
					proc();
				}
			} else {
				errorHandle(-1, "");
				return;
			}
		}
	}

	private void body() {
		//<�������>::=begin<���>{;<���>}end
		if (allToken.get(tokenPtr).getSt() == SymType.BEG) {
			tokenPtr++;
			statement();
			while (allToken.get(tokenPtr).getSt() == SymType.SEMIC || isHeadOfStatement()) {
				if (allToken.get(tokenPtr).getSt() == SymType.SEMIC) {
					tokenPtr++;
				} else {
					if (allToken.get(tokenPtr).getSt() != SymType.END) {
						errorHandle(0, "");
					}
				}
				if (allToken.get(tokenPtr).getSt() == SymType.END) {
					errorHandle(21, "");
					break;
				}
				statement();
			}
			if (allToken.get(tokenPtr).getSt() == SymType.END) {
				tokenPtr++;
			} else { //ȱ��end
				errorHandle(7, "");
				return;
			}
		} else { //ȱ��begin
			errorHandle(6, "");
			return;
		}
	}

	private void statement() {
		//<���>::=<��ֵ���> | <�������> | <��ѭ�����> | <���̵������> | <�������> | <�����> | <д���> | <��>
		if (allToken.get(tokenPtr).getSt() == SymType.IF) {
			//<�������>::=if<����>then<���>else<���>
			tokenPtr++;
			condition();
			if (allToken.get(tokenPtr).getSt() == SymType.THEN) {
				int pos1 = allPcode.getPcodePtr();
				allPcode.gen(Operator.JPC, 0, 0);
				tokenPtr++;
				statement();
				int pos2 = allPcode.getPcodePtr();
				allPcode.gen(Operator.JMP, 0, 0);
				allPcode.getAllPcode().get(pos1).setA(allPcode.getPcodePtr());
				allPcode.getAllPcode().get(pos2).setA(allPcode.getPcodePtr());
				if (allToken.get(tokenPtr).getSt() == SymType.ELS) {
					tokenPtr++;
					statement();
					allPcode.getAllPcode().get(pos2).setA(allPcode.getPcodePtr());
				}
			} else {
				errorHandle(8, "");
				return;
			}
		} else if (allToken.get(tokenPtr).getSt() == SymType.WHI) {
			//<��ѭ�����>::=while<����>do<���>
			int pos1 = allPcode.getPcodePtr();
			tokenPtr++;
			condition();
			if (allToken.get(tokenPtr).getSt() == SymType.DO) {
				int pos2 = allPcode.getPcodePtr();
				allPcode.gen(Operator.JPC, 0, 0);
				tokenPtr++;
				statement();
				allPcode.gen(Operator.JMP, 0, pos1);
				allPcode.getAllPcode().get(pos2).setA(allPcode.getPcodePtr());
			} else {
				errorHandle(9, "");
				return;
			}
		} else if (allToken.get(tokenPtr).getSt() == SymType.CAL) {
			//<���̵������>::=call<��ʶ��>
			tokenPtr++;
			int count = 0; //������Ŀ
			PerSymbol tmp;
			if (allToken.get(tokenPtr).getSt() == SymType.SYM) {
				String name = allToken.get(tokenPtr).getValue();
				if (allSymbol.isPreExists(name, level)) {
					tmp = allSymbol.getSymbol(name);
					if (tmp.getType() == allSymbol.getProc()) {
						allPcode.gen(Operator.CAL, level - tmp.getLevel(), tmp.getValue());;
					} else {
						errorHandle(11, "");
						return;
					}
				} else { //�����ڸù���
					errorHandle(10, "");
					return;
				}
				tokenPtr++;
				
			} else {
				errorHandle(1, "");
				return;
			}
		} else if (allToken.get(tokenPtr).getSt() == SymType.REA) {
			//<�����>::=read'('<��ʶ��>{,<��ʶ��>}')'
			tokenPtr++;
			if (allToken.get(tokenPtr).getSt() == SymType.LBR) {
				tokenPtr++;
				if (allToken.get(tokenPtr).getSt() == SymType.SYM) {
					String name = allToken.get(tokenPtr).getValue();
					if (!allSymbol.isPreExists(name, level)) {
						errorHandle(10, "");
						return;
					} else {
						PerSymbol tmp = allSymbol.getSymbol(name);
						if (tmp.getType() == allSymbol.getVar()) {
							allPcode.gen(Operator.OPR, 0, 16);
							allPcode.gen(Operator.STO, level - tmp.getLevel(), tmp.getAddress());
						} else {
							errorHandle(12, "");
							return;
						}
					}
				}
				tokenPtr++;
				while (allToken.get(tokenPtr).getSt() == SymType.COMMA) {
					tokenPtr++;
					if (allToken.get(tokenPtr).getSt() == SymType.SYM) {
						String name = allToken.get(tokenPtr).getValue();
						if (!allSymbol.isPreExists(name, level)) {
							errorHandle(10, "");
							return;
						} else {
							PerSymbol tmp = allSymbol.getSymbol(name);
							if (tmp.getType() == allSymbol.getVar()) {
								allPcode.gen(Operator.OPR, 0, 16);
								allPcode.gen(Operator.STO, level - tmp.getLevel(), tmp.getAddress());
							} else {
								errorHandle(12, "");
								return;
							}
						}
						tokenPtr++;
					} else {
						errorHandle(1, "");
						return;
					}
				}
				if (allToken.get(tokenPtr).getSt() == SymType.RBR) {
					tokenPtr++;
				} else {
					errorHandle(5, "");
					return;
				}
			} else {
				errorHandle(4, "");
				return;
			}
		} else if (allToken.get(tokenPtr).getSt() == SymType.WRI) {
			//<д���>::=write '('<���ʽ>{,<���ʽ>}')'
			tokenPtr++;
			if (allToken.get(tokenPtr).getSt() == SymType.LBR) {
				tokenPtr++;
				expression();
				allPcode.gen(Operator.OPR, 0, 14);
				while (allToken.get(tokenPtr).getSt() == SymType.COMMA) {
					tokenPtr++;
					expression();
					allPcode.gen(Operator.OPR, 0, 14);
				}
				allPcode.gen(Operator.OPR, 0, 15);
				if (allToken.get(tokenPtr).getSt() == SymType.RBR) {
					tokenPtr++;
				} else { //ȱ��)
					errorHandle(5, "");
					return;
				}
			} else { //ȱ�٣�
				errorHandle(4, "");
			}
		} else if (allToken.get(tokenPtr).getSt() == SymType.BEG) {
			//<�������>::=begin<���>{;<���>}end
			body();
		} else if (allToken.get(tokenPtr).getSt() == SymType.SYM) {
			//<��ֵ���>::=<��ʶ��>:=<���ʽ>
			String name = allToken.get(tokenPtr).getValue();
			tokenPtr++;
			if (allToken.get(tokenPtr).getSt() == SymType.CEQU || allToken.get(tokenPtr).getSt() == SymType.EQU || allToken.get(tokenPtr).getSt() == SymType.COL) {
				if (allToken.get(tokenPtr).getSt() == SymType.EQU || allToken.get(tokenPtr).getSt() == SymType.COL) {
					errorHandle(3, "");
				}
				tokenPtr++;
				expression();
				if (!allSymbol.isPreExists(name, level)) {
					errorHandle(14, name);
					return;
				} else {
					PerSymbol tmp = allSymbol.getSymbol(name);
					if (tmp.getType() == allSymbol.getVar()) {
						allPcode.gen(Operator.STO, level - tmp.getLevel(), tmp.getAddress());
					} else {
						errorHandle(13, name);
						return;
					}
				}
			} else {
				errorHandle(3, "");
				return;
			}
		} else if (allToken.get(tokenPtr).getSt() == SymType.REP) {
			//<�ظ����> ::= repeat<���>{;<���>}until<����>
			tokenPtr++;
			int pos = allPcode.getPcodePtr();
			statement();
			while (allToken.get(tokenPtr).getSt() == SymType.SEMIC || isHeadOfStatement()) {
				if (isHeadOfStatement()) {
					errorHandle(1, "");
				} else {
					tokenPtr++;
				}
				if (allToken.get(tokenPtr).getSt() == SymType.UNT) {
					errorHandle(22, "");
					break;
				}
				tokenPtr++;
				statement();
			}
			if (allToken.get(tokenPtr).getSt() == SymType.UNT) {
				tokenPtr++;
				condition();
				allPcode.gen(Operator.JPC, 0, pos);
			} else {
				errorHandle(19, "");
				return;
			}
		} else {
			errorHandle(1, "");
			return;
		}
	}

	private void condition() {
		//<����>::=<���ʽ><��ϵ�����><���ʽ> | odd<���ʽ>
		if (allToken.get(tokenPtr).getSt() == SymType.ODD) {
			allPcode.gen(Operator.OPR, 0, 6);
			tokenPtr++;
			expression();
		} else {
			expression();
			SymType tmp = allToken.get(tokenPtr).getSt();
			tokenPtr++;
			expression();
			if (tmp == SymType.EQU) { //��������Ƿ����
				allPcode.gen(Operator.OPR, 0, 8);
			} else if (tmp == SymType.NEQE) { //��������Ƿ񲻵�
				allPcode.gen(Operator.OPR, 0, 9);
			} else if (tmp == SymType.LES) { //С��
				allPcode.gen(Operator.OPR, 0, 10);
			} else if (tmp == SymType.LARE) { //���ڵ���
				allPcode.gen(Operator.OPR, 0, 11);
			} else if (tmp == SymType.LAR) { //����
				allPcode.gen(Operator.OPR, 0, 12);
			} else if (tmp == SymType.LESE) { //С�ڵ���
				allPcode.gen(Operator.OPR, 0, 13);
			} else { //���Ϸ��ıȽ������
				errorHandle(2, ""); 
			}
		}
	}

	private void expression() {
		//<���ʽ>::=[+|-]<��>{<�ӷ������><��>}
		//<�ӷ������>::=+|-
		SymType tmp = allToken.get(tokenPtr).getSt();
		if (tmp == SymType.ADD || tmp == SymType.SUB) {
			tokenPtr++;
		}
		term();
		if (tmp == SymType.SUB) {
			allPcode.gen(Operator.OPR, 0, 1);
		}
		while (allToken.get(tokenPtr).getSt() == SymType.ADD || allToken.get(tokenPtr).getSt() == SymType.SUB) {
			tmp = allToken.get(tokenPtr).getSt();
			tokenPtr++;
			term();
			if (tmp == SymType.ADD) {
				allPcode.gen(Operator.OPR, 0, 2);
			} else if (tmp == SymType.SUB) {
				allPcode.gen(Operator.OPR, 0, 3);
			}
		}
	}

	private void term() {
		//<��>::=<����>{<�˷������><����>}
		//<�˷������>::=*|/
		factor();
		while (allToken.get(tokenPtr).getSt() == SymType.MUL || allToken.get(tokenPtr).getSt() == SymType.DIV) {
			SymType tmp = allToken.get(tokenPtr).getSt();
			tokenPtr++;
			factor();
			if (tmp == SymType.MUL) {
				allPcode.gen(Operator.OPR, 0, 4);
			} else if (tmp == SymType.DIV) {
				allPcode.gen(Operator.OPR, 0, 5);
			}
		}
	}

	private void factor() {
		//<����>::=<��ʶ��> | <�޷�������> | '('<���ʽ>')'
		if (allToken.get(tokenPtr).getSt() == SymType.CONST) {
			allPcode.gen(Operator.LIT, 0, Integer.parseInt(allToken.get(tokenPtr).getValue()));
			tokenPtr++;
		} else if (allToken.get(tokenPtr).getSt() == SymType.LBR) {
			tokenPtr++;
			expression();
			if (allToken.get(tokenPtr).getSt() == SymType.RBR) {
				tokenPtr++;
			} else { //ȱ��������
				errorHandle(5, "");
			}
		} else if (allToken.get(tokenPtr).getSt() == SymType.SYM) {
			String name = allToken.get(tokenPtr).getValue();
			if (! allSymbol.isPreExists(name, level)) {
				errorHandle(10, "");
				return;
			} else {
				PerSymbol tmp = allSymbol.getSymbol(name);
				if (tmp.getType() == allSymbol.getVar()) {
					allPcode.gen(Operator.LOD, level - tmp.getLevel(), tmp.getAddress());
				} else if (tmp.getType() == allSymbol.getCon()) {
					allPcode.gen(Operator.LIT, 0, tmp.getValue());
				} else {
					errorHandle(12, "");
					return;
				}
			}
			tokenPtr++;
		} else {
			errorHandle(1, "");
			return;
		}
	}

	private boolean isHeadOfStatement() {
		return (allToken.get(tokenPtr).getSt() == SymType.IF ||
			allToken.get(tokenPtr).getSt() == SymType.WHI ||
			allToken.get(tokenPtr).getSt() == SymType.CAL ||
			allToken.get(tokenPtr).getSt() == SymType.REA ||
			allToken.get(tokenPtr).getSt() == SymType.WRI ||
			allToken.get(tokenPtr).getSt() == SymType.BEG ||
			allToken.get(tokenPtr).getSt() == SymType.SYM ||
			allToken.get(tokenPtr).getSt() == SymType.REP);
	}

	private void errorHandle(int k, String name) {
		errorHappen = true;
		String error = "";
		switch(k) {
			case -1: //�������岻��const��ͷ���������岻��var��ͷ
				error = "Error happened in line " + allToken.get(tokenPtr).getLine() + ":" + "wrong token";
				break;
			case 0: //ȱ�ٷֺ�
				if (allToken.get(tokenPtr).getSt() == SymType.SYM) {
					error = "Error happened in line " + allToken.get(tokenPtr).getLine() + ":" + "Missing ; before " + allToken.get(tokenPtr).getValue(); 
				} else {
					error = "Error happened in line " + allToken.get(tokenPtr).getLine() + ":" + "Missing ; before " + allToken.get(tokenPtr).getSt(); 
				}
				break;
			case 1: //��ʶ�����Ϸ�
				error = "Error happened in line " + allToken.get(tokenPtr).getLine() + ":" + "Identifier illegal";
				break;
			case 2: //���Ϸ��ıȽϷ�
				error = "Error happened in line " + allToken.get(tokenPtr).getLine() + ":" + "illegal compare symbol"; 
				break;
			case 3: //������ֵû��=
				error = "Error happened in line " + allToken.get(tokenPtr).getLine() + ":" + "Const assign must be =";
				break;
			case 4: //ȱ�٣�
				error = "Error happened in line " + allToken.get(tokenPtr).getLine() + ":" + "Missing ("; 
				break;
			case 5: //ȱ�٣�
				error = "Error happened in line " + allToken.get(tokenPtr).getLine() + ":" + "Missind )";
				break;
			case 6: //ȱ��begin
				error = "Error happened in line " + allToken.get(tokenPtr).getLine() + ":" + "Missing begin";
				break;
			case 7: //ȱ��end
				error = "Error happened in line " + allToken.get(tokenPtr).getLine() + ":" + "Missing end";
				break;
			case 8: //ȱ��then
				error = "Error happened in line " + allToken.get(tokenPtr).getLine() + ":" + "Missing then";
				break;
			case 9: //ȱ��do
				error = "Error happened in line " + allToken.get(tokenPtr).getLine() + ":" + "Missing do";
				break;
			case 10: //call, write, read����У������ڱ�ʶ��
				error = "Error happened in line " + allToken.get(tokenPtr).getLine() + ":" + "Not exist" + allToken.get(tokenPtr).getValue();
				break;
			case 11: //�ñ�ʶ������proc����
				error = "Error happened in line " + allToken.get(tokenPtr).getLine() + ":" + allToken.get(tokenPtr).getValue() + "is not a procedure"; 
				break;
			case 12: //read, write����У��ñ�ʶ������var����
				error = "Error happened in line " + allToken.get(tokenPtr).getLine() + ":" + allToken.get(tokenPtr).getValue() + "is not a variable";
				break;
			case 13: //��ֵ����У��ñ�ʶ������var����
				error = "Error happened in line " + allToken.get(tokenPtr).getLine() + ":" + name + "is not a varible";
				break;
			case 14: //��ֵ����У��ñ�ʶ��������
				error = "Error happened in line " + allToken.get(tokenPtr).getLine() + ":" + "not exist " + name; 
				break;
			case 15: //�ñ�ʶ���Ѵ���
				error = "Error happened in line " + allToken.get(tokenPtr).getLine() + ":" + "Already exist " + name; 
				break;
			case 16: //���ú�����������
				error = "Error happened in line " + allToken.get(tokenPtr).getLine() + ":" + "Number of parameters of procedure " + name + "is incorrect"; 
				break;
			case 17: //ȱ��.
				error = "Error happened in line " + allToken.get(tokenPtr).getLine() + ":" + "Missing .";
				break;
			case 18: //�������
				error = "Error happened in line " + allToken.get(tokenPtr).getLine() + ":" + "too much code after .";
				break;
			case 19: //ȱ��until
				error = "Error happened in line " + allToken.get(tokenPtr).getLine() + ":" + "Missing until"; 
				break;
			case 20: //��ֵ��ӦΪ��=
				error = "Error happened in line " + allToken.get(tokenPtr).getLine() + ":" + "Assign must be :=";
				break;
			case 21: //endǰ���ˣ�
				error = "Error happened in line " + allToken.get(tokenPtr).getLine() + ":" + "; is no need before end";
				break;
			case 22: //untilǰ���ˣ�
				error = "Error happened in line " + allToken.get(tokenPtr).getLine() + ":" + "; is no need before ubtil";
				break;
			case 23: //ȱ��,
				error = "Error happened in line " + allToken.get(tokenPtr).getLine() + ":" + "Missing ,";
				break;
		}
		errorMessage.add(error);
	}

	public List<String> getErrorMessage() {
		return errorMessage;
	}

	public void showAllToken() {
		for (int i = 0; i < allToken.size(); i++) {
			System.out.println(allToken.get(i).getSt() + " " + allToken.get(i).getLine() + " " + allToken.get(i).getValue());
		}
	}

	public List<Token> getAllToken() {
		return allToken;
	}

	public void showAllSymbol() {
		List<PerSymbol> display = allSymbol.getAllSymbol();
		for (int i = 0; i < display.size(); i++) {
			System.out.println(display.get(i).getType() + " " +
				display.get(i).getName() + " " +
				display.get(i).getValue() + " " +
				display.get(i).getLevel() + " " +
				display.get(i).getAddress());
		}
	}	

	public List<PerSymbol> getAllSymbol() {
		return allSymbol.getAllSymbol();
	}

	public void showAllPcode() {
		List<PerPcode> display = allPcode.getAllPcode();
		for (int i = 0; i < display.size(); i++) {
			System.out.print(i + " " + display.get(i).getF() + "     ");
			System.out.println(" " + display.get(i).getL() + " " +display.get(i).getA());
		}
	}

	public List<PerPcode> getAllPcode() {
		return allPcode.getAllPcode();
	}

	public List<String> interpreter(List<Integer> input) {
		Interpreter one = new Interpreter(input);
		one.setAllPcode(allPcode);
		one.interpreter();
		return one.getOutput();
	}

	public void interpreter() {
		Interpreter one = new Interpreter();
		one.setAllPcode(allPcode);
		one.interpreter();
	}
}
