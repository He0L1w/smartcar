%�ж���Ӧ������
function ReceiveCallback( obj,event)     %�����ж���Ӧ����  
   global s;
   global I;
   global Route;
   a = fscanf(s);       % �������� ����Ȼ��Ҳ���Բ����ա��Ѵ�������Ϳ����ˡ��������ֻ�ǽ̸�������ü����ִ��Arduino������ķ�����
   fwrite(s,Route(I));    %д���ڣ���Arduino����������ʱû����ֹͣ������ԼӸ�����I��if����жϡ�
   I = I+1;   % ��¼�������е��������һ��  ����Ȼ��Ҳ�������������飬��ֻ��һ����ͨ�ĺ���
end  