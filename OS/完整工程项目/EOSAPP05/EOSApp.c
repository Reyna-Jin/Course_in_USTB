#include "EOSApp.h"

//
// main �������������壺
// argc - argv ����ĳ��ȣ���С����Ϊ 1��argc - 1 Ϊ�����в�����������
// argv - �ַ���ָ�����飬���鳤��Ϊ�����в������� + 1������ argv[0] �̶�ָ��ǰ
//        ������ִ�еĿ�ִ���ļ���·���ַ�����argv[1] ��������ָ��ָ�����������
//        ������
//        ����ͨ������������ "a:\hello.exe -a -b" �������̺�hello.exe �� main ��
//        ���Ĳ��� argc ��ֵΪ 3��argv[0] ָ���ַ��� "a:\hello.exe"��argv[1] ָ��
//        �����ַ��� "-a"��argv[2] ָ������ַ��� "-b"��
//
int main(int argc, char* argv[])
{
	//
	// �����Ҫ�ڵ���Ӧ�ó���ʱ�ܹ����Խ����ں˲���ʾ��Ӧ��Դ�룬
	// ����ʹ�� EOS �ں���Ŀ����������ȫ�汾�� SDK �ļ��У�Ȼ
	// ��ʹ�øո����ɵ� SDK �ļ��и��Ǵ�Ӧ�ó�����Ŀ�е� SDK �ļ�
	// �У����� EOS �ں���Ŀ�ڴ����ϵ�λ�ò��ܸı䡣
	//

	/* TODO: �ڴ˴�����Լ��Ĵ��� */
	// ��Ӧ�ó�������з�������ҳ���ͷ�����ҳ
	INT *p;
	if(p=VirtualAlloc(0,sizeof(int),MEM_RESERVE|MEM_COMMIT)){//����API����VirtualAlloc������һ�����ͱ�������Ŀռ䣬��
	// ʹ��һ�����ͱ�����ָ��ָ������ռ�
		printf("Allocate %d bytes virtual memory at 0x%x.\n\n",sizeof(int),p);
		printf("Virtual memory original value:0x%x\n",*p);
		*p=0xFFFFFFFF;
		printf("virtual memory new value:0x%x\n\n",*p);
		printf("Wait for 10 second\n\n");
		Sleep(10000);	//����API����Sleep���ȴ�10����
		if(VirtualFree(p,0,MEM_RELEASE))//����API����VirtualFree���ͷ�֮ǰ��������ͱ����Ŀռ�
			printf("Release virtual memory success!\n\n");
		else
			printf("Release error\n");
		printf("Endless loop!\n");
		for(;;){
		}
	}
	else{
		printf("Error\n");
		return -1;//�����ܳɹ����䣬��ӡ��erroe������-1
	}
	
	printf("Hello world!\n");

	return 0;
}
